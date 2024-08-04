#![no_std]

extern crate alloc;

use zephyr::device_dt_get;
use zephyr::gpio_dt_spec_get;
use zephyr::gpio_dt_spec_get_by_idx;
use zephyr::kconfig::CONFIG_BOARD;
use zephyr::kernel;
use zephyr::printk;

use zephyr::drivers::gpio::{GpioFlags, GpioPin};
use zephyr::*; // Wildcard import to bring in nested macros, maybe there is a better way

#[no_mangle]
extern "C" fn rust_main() {
    printk!("\nBoard: {}\n", CONFIG_BOARD);

    let gpio_pin = GpioPin::new(gpio_dt_spec_get!(dt_alias!(led0), gpios));

    gpio_pin
        .configure(GpioFlags::OutputActive)
        .expect("Failed to configure pin.");

    loop {
        kernel::msleep(1000);

        gpio_pin.toggle().expect("Failed to toggle pin.");
    }
}
