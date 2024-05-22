# freertos_task_notification
demo of freertos direct task notification using uart interrupt


git clone

cd directory

mkdir build

cd build

cmake ..

make

sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program program_name.elf verify reset exit"

you must turn on/off power to pico for the new firmware to take effect
