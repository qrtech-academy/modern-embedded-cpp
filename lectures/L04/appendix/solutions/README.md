# Exercise solutions

Reference implementations for the exercises in [Appendix C](../c_exercises.md).

**Note!** Try to solve the exercises yourself before looking at the solutions.

## Solutions
* [raw_factory](./raw_factory/include/app/logic/logic.h): Application logic implemented using raw pointers and a [raw-pointer factory](./raw_factory/include/driver/factory/interface.h).
* [smart_factory](./smart_factory/include/app/logic/logic.h): Application logic implemented using smart pointers and a [smart-pointer factory](./smart_factory/include/driver/factory/interface.h).

## Selecting the factory
The reference solution uses the `STUB` macro in `main.cpp` to switch between the ESP32-S3 factory and the stub factory. This is only for convenience and was not required by the exercise.

The example application can be built using either the ESP32-S3 drivers or the stub drivers.

By default, the stub drivers are used due to the macro `STUB` being defined in `main.cpp`:

```cpp
#define STUB
```

To use the ESP32-S3 drivers instead, comment out or remove the definition:

```cpp
// #define STUB
```

The application logic (`app::logic::Logic`) does not need to be modified. Only the selected factory changes.

## Reflection
**What code in `Logic` had to change when switching from the ESP32-S3 factory to the stub factory?**  
* No code in `Logic` had to change.
* The logic class depends only on the abstract factory interface and the abstract serial interface, not on the concrete ESP32-S3 or stub implementations.
* This means the application logic can use either real hardware drivers or stub drivers without being modified.

**What code in `main()` had to change?**  
* Only the concrete factory type created in `main()` had to change.
* Instead of creating a `driver::factory::Esp32s3`, `main()` creates a `driver::factory::Stub`. 
* The rest of the code can stay the same, because both factories implement the same factory interface.

---
