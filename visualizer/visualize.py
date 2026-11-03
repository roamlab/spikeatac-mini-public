#!/usr/bin/env python3
"""Standalone (non-ROS-package) live viz for spikeatac-mini.

Overlays the 4 PVDF taxels and 4 capacitive taxels on a photo of the
spikeatac-mini finger, in the same window:

  - PVDF (signed, dead-zone around 0):        filled circle, red -> yellow -> green
  - Capacitive (baseline-zeroed, unsigned):    blue ring, sized by |signal|,
                                                vanishes near baseline

This is a plain script, not a ROS package/plugin -- just `python3 visualize.py`.
It subscribes directly to the raw feedback topics and does its own baseline
zeroing (median of the first N samples), so no separate zero_signals.py node
or colcon build is required.
"""

import argparse
import os
import threading

import cv2
import numpy as np
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy
from std_msgs.msg import Int32MultiArray, UInt16MultiArray

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DEFAULT_IMAGE = os.path.join(SCRIPT_DIR, "finger.jpg")

# Physical taxel sites: a 2x2 grid over the sensing pad visible on the
# cropped photo. PVDF and capacitive report their 4 taxels in different
# orders, so each gets its own index -> site mapping onto the same 4
# physical locations.
_TOP_LEFT = (385, 506)
_TOP_RIGHT = (565, 506)
_BOTTOM_LEFT = (385, 814)
_BOTTOM_RIGHT = (565, 814)

PVDF_SITES = [
    _BOTTOM_RIGHT,  # 0
    _TOP_RIGHT,     # 1
    _BOTTOM_LEFT,   # 2
    _TOP_LEFT,      # 3
]

CAP_SITES = [
    _TOP_LEFT,      # 0
    _BOTTOM_LEFT,   # 1
    _TOP_RIGHT,     # 2
    _BOTTOM_RIGHT,  # 3
]

PVDF_DEAD_ZONE = 60  # lower = soft touches register sooner
CAP_BLUE = (255, 90, 20)  # BGR
CAP_DISAPPEAR_BELOW = 8  # |zeroed cap value| below this draws no ring


def clamp(v, lo, hi):
    return max(lo, min(hi, v))


def pvdf_color(v, vmin, vmax):
    """Red (low) -> yellow (0, dead zone) -> green (high)."""
    if -PVDF_DEAD_ZONE <= v <= PVDF_DEAD_ZONE:
        t = 0.5
    elif v < -PVDF_DEAD_ZONE:
        t = 0.5 * (v - vmin) / (-PVDF_DEAD_ZONE - vmin)
        t = clamp(t, 0.0, 0.5)
    else:
        t = 0.5 + 0.5 * (v - PVDF_DEAD_ZONE) / (vmax - PVDF_DEAD_ZONE)
        t = clamp(t, 0.5, 1.0)

    if t <= 0.5:
        a = t / 0.5
        r, g, b = 255, int(round(255 * a)), 0
    else:
        a = (t - 0.5) / 0.5
        r, g, b = int(round(255 * (1.0 - a))), 255, 0
    return (b, g, r)  # BGR for OpenCV


def radius_for_value(v, vmin, vmax, rmin, rmax, dead_zone=0):
    if vmax == vmin:
        return rmin
    if dead_zone and -dead_zone <= v <= dead_zone:
        t = 0.5
    elif dead_zone and v < -dead_zone:
        t = clamp(0.5 * (v - vmin) / (-dead_zone - vmin), 0.0, 0.5)
    elif dead_zone:
        t = clamp(0.5 + 0.5 * (v - dead_zone) / (vmax - dead_zone), 0.5, 1.0)
    else:
        t = clamp((v - vmin) / (vmax - vmin), 0.0, 1.0)
    return int(round(rmin + t * (rmax - rmin)))


class Baseline:
    """Running median baseline over the first `size` samples, then subtracts it."""

    def __init__(self, size):
        self.size = size
        self.buffer = []
        self.median = None

    def zero(self, data):
        data = np.asarray(data, dtype=np.float64)
        if self.median is None:
            self.buffer.append(data)
            if len(self.buffer) >= self.size:
                self.median = np.median(np.array(self.buffer), axis=0)
            return np.zeros_like(data)
        return data - self.median


class SpikeatacMiniVizNode(Node):
    def __init__(self, pvdf_topic, cap_topic, baseline_size):
        super().__init__("spikeatac_mini_visualizer")

        qos = QoSProfile(depth=10, reliability=ReliabilityPolicy.BEST_EFFORT)

        self.lock = threading.Lock()
        self.pvdf_baseline = Baseline(baseline_size)
        self.cap_baseline = Baseline(baseline_size)
        self.pvdf_zeroed = None
        self.cap_zeroed = None

        self.create_subscription(Int32MultiArray, pvdf_topic, self._on_pvdf, qos)
        self.create_subscription(UInt16MultiArray, cap_topic, self._on_cap, qos)

    def _on_pvdf(self, msg):
        zeroed = self.pvdf_baseline.zero(msg.data)
        with self.lock:
            self.pvdf_zeroed = zeroed

    def _on_cap(self, msg):
        zeroed = self.cap_baseline.zero(msg.data)
        with self.lock:
            self.cap_zeroed = zeroed

    def latest(self):
        with self.lock:
            return self.pvdf_zeroed, self.cap_zeroed


def render_frame(base, pvdf, cap, pvdf_range, cap_range):
    frame = base.copy()
    pvdf_min, pvdf_max = pvdf_range
    cap_min, cap_max = cap_range

    # PVDF filled dots drawn first, capacitive rings drawn on top -- otherwise
    # a ring smaller than the (always-visible, radius >= 14) PVDF dot would be
    # painted over and vanish even when its signal is well above threshold.
    if pvdf is not None:
        for i in range(min(len(PVDF_SITES), len(pvdf))):
            v = clamp(pvdf[i], pvdf_min, pvdf_max)
            color = pvdf_color(v, pvdf_min, pvdf_max)
            r = radius_for_value(v, pvdf_min, pvdf_max, 14, 60, dead_zone=PVDF_DEAD_ZONE)
            cv2.circle(frame, PVDF_SITES[i], r, color, -1, cv2.LINE_AA)

    if cap is not None:
        for i in range(min(len(CAP_SITES), len(cap))):
            v = clamp(cap[i], cap_min, cap_max)
            mag = abs(v)
            if mag < CAP_DISAPPEAR_BELOW:
                continue  # small signal: no ring
            r = radius_for_value(mag, 0, cap_max, 16, 85)
            cv2.circle(frame, CAP_SITES[i], r, CAP_BLUE, 4, cv2.LINE_AA)

    return frame


def main():
    global PVDF_SITES, CAP_SITES, PVDF_DEAD_ZONE, CAP_DISAPPEAR_BELOW

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--image", default=DEFAULT_IMAGE, help="Base finger photo")
    parser.add_argument("--pvdf-topic", default="/satmini_pvdf_feedback")
    parser.add_argument("--cap-topic", default="/satmini_capacitive_feedback")
    parser.add_argument("--baseline-samples", type=int, default=300,
                         help="Samples used to compute the zero-baseline median")
    parser.add_argument("--pvdf-range", type=int, default=1000,
                         help="+/- range (about 0) for PVDF color/size mapping")
    parser.add_argument("--cap-range", type=int, default=1000,
                         help="+/- range (about 0) for capacitive color/size mapping")
    parser.add_argument("--pvdf-dead-zone", type=int, default=PVDF_DEAD_ZONE,
                         help="|zeroed pvdf value| below this stays neutral yellow (lower = more sensitive)")
    parser.add_argument("--cap-disappear-below", type=int, default=CAP_DISAPPEAR_BELOW,
                         help="|zeroed cap value| below this draws no ring (lower = more sensitive)")
    parser.add_argument("--scale", type=float, default=1.0, help="Display scale factor")
    args = parser.parse_args()

    PVDF_DEAD_ZONE = args.pvdf_dead_zone
    CAP_DISAPPEAR_BELOW = args.cap_disappear_below

    base = cv2.imread(args.image, cv2.IMREAD_COLOR)
    if base is None:
        raise SystemExit(f"Could not load image: {args.image}")
    if args.scale != 1.0:
        base = cv2.resize(base, None, fx=args.scale, fy=args.scale, interpolation=cv2.INTER_LINEAR)

        def scaled(sites):
            return [(int(round(x * args.scale)), int(round(y * args.scale))) for x, y in sites]

        PVDF_SITES = scaled(PVDF_SITES)
        CAP_SITES = scaled(CAP_SITES)

    rclpy.init()
    node = SpikeatacMiniVizNode(args.pvdf_topic, args.cap_topic, args.baseline_samples)

    def spin():
        try:
            rclpy.spin(node)
        except Exception:
            pass  # benign race with shutdown on Ctrl+C

    spin_thread = threading.Thread(target=spin, daemon=True)
    spin_thread.start()

    window = "spikeatac-mini viz"
    cv2.namedWindow(window, cv2.WINDOW_NORMAL)
    cv2.resizeWindow(window, base.shape[1], base.shape[0])

    pvdf_range = (-args.pvdf_range, args.pvdf_range)
    cap_range = (-args.cap_range, args.cap_range)

    print(f"[viz] recording baseline over {args.baseline_samples} samples per topic, hold the finger still...")

    try:
        while rclpy.ok():
            pvdf, cap = node.latest()
            frame = render_frame(base, pvdf, cap, pvdf_range, cap_range)
            cv2.imshow(window, frame)
            key = cv2.waitKey(33) & 0xFF
            if key == ord("q"):
                break
            try:
                if cv2.getWindowProperty(window, cv2.WND_PROP_VISIBLE) < 1:
                    break
            except cv2.error:
                break  # window was closed via its close button
    except KeyboardInterrupt:
        pass
    finally:
        cv2.destroyAllWindows()
        node.destroy_node()
        rclpy.try_shutdown()
        spin_thread.join(timeout=1.0)


if __name__ == "__main__":
    main()
