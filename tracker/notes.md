- WIP sleep_manager.h with peripherals + check with Gaute what recommends
- WIP sleep_manager.cpp
- test for all
- check power consumption

- add tests time_manager
- add tests print_utils
- add formatted print of firmware configuration
- define a "dummy" serial that does nothing, to de-activate all serial printing; may need some form of macro or constexpr to switch between true serial and our dummy serial...
- have some counters to count number of users of each kind of peripherals, for being able to switch on / off; write some wrappers to automate make sure on / off? start_use, stop_use, force_turn_off, with a counter.