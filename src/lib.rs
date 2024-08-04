#![no_std]

extern crate alloc;

use zephyr::device_dt_get;
use zephyr::gpio_dt_spec_get;
use zephyr::gpio_dt_spec_get_by_idx;
use zephyr::kernel;
use zephyr::printk;

use zephyr::drivers::gpio::{GpioFlags, GpioPin};
use zephyr::*; // Wildcard import to bring in nested macros, maybe there is a better way

// Reference the Zephyr crate so that the panic handler gets used.  This is only needed if no
// symbols from the crate are directly used.
extern crate zephyr;
#[no_mangle]
extern "C" fn rust_main() {
    // add code here
    //printk!("What hath god wraught! {}", 42);
    printk!("Hello world from Rust on {}", zephyr::kconfig::CONFIG_BOARD);

    let gpio_pin = GpioPin::new(gpio_dt_spec_get!(dt_alias!(led0), gpios));

    gpio_pin
        .configure(GpioFlags::OutputActive)
        .expect("Failed to configure pin.");

    loop {
        kernel::msleep(1000);

        gpio_pin.toggle().expect("Failed to toggle pin.");
    }
}
