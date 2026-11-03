# JLCPCB Ordering Instructions

Instructions for ordering all PCBs for the stack from JLCPCB.

Screenshots of the order options live in [`jlcpcb_screenshots/`](jlcpcb_screenshots/) in this repo.
## Boards overview

| # | Board | Role | Type | Assembled? |
|---|-------|------|------|------------|
| 1 | `board1` | Interfaces with the PVDF. **Special stackup (embedded capacitors).** | Rigid | Yes |
| 2 | `board2` | Middle board in the stack | Rigid | Yes |
| 3 | `board3` | Outermost board in the stack; connector to control board | Rigid | Yes |
| 4 | `control board` | Teensy based control board | Rigid | Yes (though I usually populate the THT components by hand) |
| 5 | `PVDF flex cable` | PVDF sensor electrodes | Flex | No |
| 6 | `CAP flex cable` | Capacitive sensor electrodes | Flex | No |

That is 6 boards total: 4 rigid assembled boards and 2 flex boards with no assembly.

> **Order of operations:** Part 1 (pre-order out-of-stock parts) comes first, because some parts have long lead times. **The pre-ordered parts must arrive at the JLCPCB warehouse before you can place the PCBA order for that board.** Then do Part 2 (place the PCB/PCBA orders).

---

## Part 1: Pre-order out-of-stock components

Do this **once per rigid board** (board1, board2, board3, control board). The quantity of each part depends on the number of boards you plan to order. The flex boards are not assembled, so they have nothing to pre-order.

### Steps

1. Go to the JLCPCB BOM tool: <https://jlcpcb.com/parts/bom-tool>
2. Upload the BOM for the board.
3. Review each line's availability (see the column definitions below).
4. For every part that is out of stock (or in limited stock), set the quantity to match your planned board count (plus any overage you want) and **add it to the parts cart**.
5. Repeat for each of the 4 rigid boards.
6. Check out the parts cart to pre-order the parts.
7. **Wait for the pre-ordered parts to arrive** at the JLCPCB warehouse (they will show up under "My parts lib"). You cannot place the PCBA order for a board until its parts have arrived.

> **Tip:** you can pre-order more parts than you need for the build. The extras stay reserved in your parts library, giving you personal stock that is ready to go for future orders.

### Understanding the BOM tool columns

- **Stock**: parts that are in JLCPCB stock and can be used in your order, but are **not reserved for you**. If stock is limited, pre-order those parts to reserve them.
- **My parts lib**: parts you have **already paid for** and that are reserved for you.
- **Parts cart**: parts you have added to your shopping cart for pre-order.
- **Pre-order parts**: parts that are not in stock and must be ordered to the JLCPCB warehouse. **These have the longer lead times.**

### Things to watch for

- **DNP (do not place) parts:** the BOMs contain some DNP lines. **Do not pre-order these.** Double check each BOM before adding items to the cart.
- **Quote failures:** sometimes the quote for the JLCPCB parts pre-order fails. When that happens, place the order through the **Global Sourcing** option of the parts pre-ordering instead.
- **Global sourcing lead time:** global sourcing parts have to be reviewed and shipped from global distributors, so expect a long lead time. Start this step as early as possible.
- **Quantity per board:** since the BOM tool is run once per board, make sure shared parts across boards are not accidentally double counted or under counted.

### JLCPCB help pages on pre-ordering parts

- [What is JLCPCB Parts Pre-order Service?](https://jlcpcb.com/help/article/what-is-jlcpcb-parts-pre-order-service)
- [How to build your own parts library in JLCPCB](https://jlcpcb.com/help/article/how-to-build-your-own-parts-library-in-jlcpcb)
- [How to use JLCPCB Global Sourcing Parts Service?](https://jlcpcb.com/help/article/60-how-to-use-jlcpcb-global-sourcing-parts-service)
- [PCBA Parts Sourcing Instruction](https://jlcpcb.com/help/article/pcba-parts-sourcing-instruction) (overview of JLCPCB parts vs. global sourcing vs. pre-order vs. consigned, and which can be mixed in one order)

Key points from those pages:

- Pre-ordered parts are held in your private parts library at JLCPCB until you place a PCBA order. They can only be used for assembly (not picked up in principle), and there is no inventory cost when used for assembly.
- Non-stock extended parts must be pre-ordered and have a minimum order quantity. Basic parts normally have enough stock and don't need to be pre-ordered.
- Global sourcing parts can only be used once they have arrived at the JLCPCB warehouse.
- For the **same part** in one order, global sourcing / consigned inventory cannot be mixed with pre-order / JLCPCB stock. Different parts in the same order can use different sources.

---

## Part 2: Place the board orders

Do this once per board, starting from the JLCPCB home page: <https://jlcpcb.com>

### General workflow (rigid, assembled boards)

1. Go to <https://jlcpcb.com> and click **Order Now** / **Instant Quote**.
2. Upload the board's **Gerber** zip. Wait for the board preview to render and confirm the dimensions look right.
3. Select the PCB options (see the per-board sections below and the screenshots).
4. Enable **PCB Assembly** and choose the assembly options (see screenshots).
5. Continue to the assembly step and upload the **BOM** and **CPL**.
6. Review the component matching. Parts you pre-ordered should show as coming from **My parts lib** / reserved. Confirm DNP parts are **not** being placed.
7. On the placement preview, **check every component's rotation and position**, and correct any that are wrong (see "Correcting component rotations" below).
8. Review the quote and save to cart (or check out).

### Correcting component rotations

JLCPCB's CPL conversion often gets the rotation of some parts wrong (commonly ICs, connectors, polarized parts like diodes/LEDs/tantalum caps, and parts with non-standard footprints).

1. On the component placement preview, step through each part (or at minimum every polarized part and every IC/connector).
2. Compare pin 1 / polarity marker against the PCB silkscreen and your design files.
3. Where wrong, adjust the rotation using the rotate control in the preview until it matches.
4. Re-check after changes. Note any rotation corrections you needed so they can be fixed in the CPL source for next time.

---

### board1: PVDF interface board (SPECIAL STACKUP)

> ## IMPORTANT: board1 REQUIRES A SPECIFIC STACKUP
>
> **board1 has embedded capacitors, and its performance depends on the exact layer stackup (layer count, dielectric thicknesses, and copper weights).**
>
> **Do NOT use the default stackup.** Select the custom/specified stackup exactly as shown in the screenshots below and double check it before checking out. A wrong stackup changes the embedded capacitance and invalidates the board.

**Stackup requirements (fill in / verify against screenshots):**

| Parameter | Required value |
|-----------|----------------|
| Layer count | TODO |
| Stackup / impedance option selected | TODO |
| Dielectric thickness between embedded capacitor layers | TODO |
| Overall board thickness | TODO |
| Copper weight (outer / inner) | TODO |
| Other options that must differ from default | TODO |

Screenshots of options (including the stackup):

![board1 options 1](jlcpcb_screenshots/board1_options1.png)
![board1 options 2](jlcpcb_screenshots/board1_options2.png)
![board1 options 3](jlcpcb_screenshots/board1_options3.png)

**Before checking out board1:**

- [ ] Stackup matches the screenshot exactly
- [ ] Gerber uploaded, preview looks correct
- [ ] BOM + CPL uploaded, pre-ordered parts show as reserved, DNP parts excluded
- [ ] Rotations verified and corrected

---

### board2: middle board in the stack

Mostly generic settings. Follow the screenshots.

Screenshots of options:

![board2 options 1](jlcpcb_screenshots/board2_options1.png)
![board2 options 2](jlcpcb_screenshots/board2_options2.png)
![board2 options 3](jlcpcb_screenshots/board2_options3.png)

Checklist:

- [ ] Options match screenshots
- [ ] BOM + CPL uploaded, DNP excluded, rotations verified

---

### board3: outermost board in the stack (connector to control board)

Mostly generic settings. Follow the screenshots. Pay extra attention to the **connector** footprint and rotation, since it mates with the control board.

Screenshots of options:

![board3 options 1](jlcpcb_screenshots/board3_options1.png)
![board3 options 2](jlcpcb_screenshots/board3_options2.png)
![board3 options 3](jlcpcb_screenshots/board3_options3.png)

Checklist:

- [ ] Options match screenshots
- [ ] BOM + CPL uploaded, DNP excluded, rotations verified (especially the connector)

---

### Control board

Mostly generic settings. Follow the screenshots.

Screenshots of options:

![control board options 1](jlcpcb_screenshots/controlboard_options1.png)
![control board options 2](jlcpcb_screenshots/controlboard_options2.png)
![control board options 3](jlcpcb_screenshots/controlboard_options3.png)

Checklist:

- [ ] Options match screenshots
- [ ] BOM + CPL uploaded, DNP excluded, rotations verified

---

### Flex cables (PVDF and CAP, not assembled)

Gerber only. **Do not enable PCB Assembly.** Make sure the **flex PCB** product/option is selected (not a rigid FR4 board).

The price increases minimally for more boards, so I recommend ordering extra.

Screenshots of options (one per flex cable):

![flexcap options](jlcpcb_screenshots/flexcap_options.png)
![flexpvdf options](jlcpcb_screenshots/flexpvdf_options.png)

Checklist:

- [ ] Flex PCB option selected
- [ ] Gerber uploaded, preview looks correct
- [ ] Assembly disabled
- [ ] Quantity set to cover both cables (and spares)

