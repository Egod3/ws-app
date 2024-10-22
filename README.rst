ws-app
******
   :name: weather station app
   :relevant-api: gpio_interface

   Collect environmental weather data and send it to a server & display it locally.

Overview
********

The purpose is to collect and display environmental weather and climate data to the user.

Build and Flash
***************

- To build the application make sure you have the python virtural environment activated:

  $ source /mnt/NAS/data/git/rust_embd/oses/zephyrproject/.venv/bin/activate

- Then run ``west build`` for the board you want. example:

  $ west build -b nucleo_l476rg -p

- Run ``west flash``:

  $ west flash

Test
****

- To test the build and flash procedure was a success, you can connect to uart as follows:

  $ tio /dev/ttyACM0 -b 230400

- Run the `help` command to see what shell commands are available.
- Run `si70xx temp` or `si70xx humid` to see temperature and relative humidtiy.
