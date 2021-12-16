- implement "user stuff" wakeup sleep_manager
- review sleep_manager and all upstream
- test the sleep functionalities (all of them!) in unit tests
- make all comments to sleep_manager up to date
- ask Gaute code review
- check power consumption in actual AGT
- add a "procedure for extra checks" information

- add tests print_utils
- add formatted print of firmware configuration
- define a "dummy" serial that does nothing, to de-activate all serial printing; may need some form of macro or constexpr to switch between true serial and our dummy serial...
- have some counters to count number of users of each kind of peripherals, for being able to switch on / off; write some wrappers to automate make sure on / off? start_use, stop_use, force_turn_off, with a counter. Do this with some derived class of the base classes for each peripheral.

- add print of firmware and user configuration