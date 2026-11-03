import click
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32MultiArray, Int16MultiArray, UInt16MultiArray
import numpy as np
from scipy.signal import butter, sosfilt, sosfilt_zi

MSG_TYPES = {
    'Int32': Int32MultiArray,
    'Int16': Int16MultiArray,
    'UInt16': UInt16MultiArray
}

class ZeroSignalsNode(Node):
    def __init__(self, msg_type, input_topic='finger_data_raw', output_topic='finger_data_zeroed', buffer_size=300, median_filter_size=0, highpass=False, highpass_cutoff=80, sample_rate=1000):
        super().__init__('zero_signals_node')

        qos_profile = rclpy.qos.QoSProfile(depth=10, reliability=rclpy.qos.ReliabilityPolicy.BEST_EFFORT)

        self.sub = self.create_subscription(
            msg_type,
            input_topic,
            self.callback,
            qos_profile=qos_profile)

        self.pub = self.create_publisher(
            Int32MultiArray,
            output_topic,
            qos_profile=qos_profile)

        self.buffer_size = buffer_size
        self.median_filter_size = median_filter_size
        self.medians_array = None
        self.data_buffer = []
        self.filter_buffer = []

        # High-pass Butterworth filter setup
        self.highpass = highpass
        self.hp_sos = None
        self.hp_zi = None
        if self.highpass:
            nyquist = sample_rate / 2
            normalized_cutoff = highpass_cutoff / nyquist
            self.hp_sos = butter(2, normalized_cutoff, btype='high', output='sos')
            print(f'High-pass Butterworth filter enabled: 2nd order, {highpass_cutoff} Hz cutoff, {sample_rate} Hz sample rate')

        print(f'Recording a baseline of {self.buffer_size} samples')
        if self.median_filter_size > 0:
            print(f'Median filter enabled with window size {self.median_filter_size}')

    def callback(self, msg):
        if len(self.data_buffer) < self.buffer_size:
            self.data_buffer.append(np.array(msg.data).astype(np.int32))
        elif self.medians_array is None:
            self.medians_array = self.calculate_medians().astype(np.int32)
            print('Baseline recorded. Median values:', self.medians_array)
            print('Publishing zeroed data')
        else:
            zeroed_data = np.array(msg.data).astype(np.float64) - self.medians_array

            # Apply high-pass filter
            if self.highpass:
                if self.hp_zi is None:
                    # Initialize filter state for each channel
                    zi_single = sosfilt_zi(self.hp_sos)
                    self.hp_zi = np.zeros((self.hp_sos.shape[0], 2, len(zeroed_data)))
                    for ch in range(len(zeroed_data)):
                        self.hp_zi[:, :, ch] = zi_single * zeroed_data[ch]
                # Filter each channel with its own state
                filtered_data = np.zeros_like(zeroed_data)
                for ch in range(len(zeroed_data)):
                    filtered_data[ch], self.hp_zi[:, :, ch] = sosfilt(
                        self.hp_sos, [zeroed_data[ch]], zi=self.hp_zi[:, :, ch])
                zeroed_data = filtered_data

            zeroed_data = zeroed_data.astype(np.int32)

            if self.median_filter_size > 0:
                self.filter_buffer.append(zeroed_data)
                if len(self.filter_buffer) > self.median_filter_size:
                    self.filter_buffer.pop(0)
                if len(self.filter_buffer) >= self.median_filter_size:
                    filter_array = np.array(self.filter_buffer)
                    zeroed_data = np.median(filter_array, axis=0).astype(np.int32)
                else:
                    return  # Wait until buffer is full
            zeroed_msg = Int32MultiArray(data=zeroed_data)
            self.pub.publish(zeroed_msg)

    def calculate_medians(self):
        data_array = np.array(self.data_buffer)
        medians = np.median(data_array, axis=0)
        return medians


@click.command()
@click.option('--input-msg-type', type=click.Choice(MSG_TYPES.keys()), required=True,
              help='Message type of the input topic (Int32, Int16, UInt16)')
@click.option('--input-topic', default='finger_data_raw', show_default=True, help='Input topic name')
@click.option('--output-topic', default='finger_data_zeroed', show_default=True, help='Output topic name')
@click.option('--buffer-size', default=300, show_default=True, help='Number of samples to use for baseline calculation')
@click.option('--median-filter-size', default=0, show_default=True, help='Window size for median filter on output (0 to disable)')
@click.option('--highpass', is_flag=True, help='Enable 2nd order Butterworth high-pass filter')
@click.option('--highpass-cutoff', default=80, show_default=True, help='High-pass filter cutoff frequency in Hz')
@click.option('--sample-rate', default=2000, show_default=True, help='Sample rate in Hz (required for high-pass filter)')
def main(input_msg_type, input_topic, output_topic, buffer_size, median_filter_size, highpass, highpass_cutoff, sample_rate):
    """Zero sensor data using running median baseline."""
    rclpy.init()
    msg_type_cls = MSG_TYPES[input_msg_type]
    node = ZeroSignalsNode(msg_type_cls, input_topic, output_topic, buffer_size, median_filter_size, highpass, highpass_cutoff, sample_rate)
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
