# Use Case


1) To allow "vessel data recorder"-type logging on a very simple compute (i.e., 
one with minimal power consumption).

1) To provide a useful log of all of the most relevant data flowing through the
H5000 CPU, which is the "brain" of the B&G network system.

1) To allow operation without requiring a B&G multi-function display
Proper calibration of a B&G H5000 system is critical to realizing the full advantages
of the system. However, in the real world, this calibration is difficult.

For example, typical "paddle wheel" speed sensors&mdash;even when regularly cleaned to prevent
marine growth fouling&mdash;do not provide consistent speed readings across a range
of speeds through the water. Calibrating to correctly report the speed at, say, 7 knots
is unlikely to result in a valid speed reading at 3 knots.

B&G provides a wide variety of calibration settings to adjust the "raw" readings from
network sensors to more accurate readings for a given sensor and environmental 
conditions (wind speed, heel, wind angle, etc.).

Some professionals (and users) seem to adjust these calibrations by trial and error;
others use a program such as Expedition to log the observations being reported by
the system into an Excel-compatible `.csv` file and using Excel to analyze the data
and solve for the calibrating adjustments that result in the data "making sense"&mdash;
being internally consistent. (For example, True Wind Direction (TWD) should be
relatively constant before and after tacking or gybing. But, unless a boat's system
is optimally calibrated, it often is not.)

Instead of using Expedition to log this data, this application establishes a 
connection to the H5000 CPU websocket and requests a continuing stream of all 
available data elements. The incoming data can be stored in two formats 
(including both simultaneously):

* A `.log` file with one line for each data observation, and/or
* A `.csv` file that accumulates all observations made within a specified time interval
(default: 2 seconds) with a row for each timestamp and a column for each datum observed.

It can be run on a Windows machine (e.g., a laptop) or on a device with much lower
power consumption, such as a Raspberry Pi.

