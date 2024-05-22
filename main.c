/*

free rtos on pico RP2040
demo of direct to task notification
demo to get string from uart and convert to command
for this demo if uart sends 'led on' then led on gpio16 turns on
if uart send 'led off' then led on gpio16 turns off

*/

#include <string.h>

#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "timers.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

static char received_buffer[100];
volatile uint8_t buffer_position = 0;
uint8_t rxChar;

TaskHandle_t xTaskToNotify_UART = NULL;

void uart_task(void *pvParameters){
  // init uart_task
  (void) pvParameters;
  uart_set_irq_enables(uart0, true, false); // enable receive interrupt only on init
  
  while(true){
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // wait for uart interrupt to be notified that a character arrives
	while (uart_is_readable(uart0)){
     rxChar = uart_getc(uart0);
	  received_buffer[buffer_position++] = rxChar;
	  if ((rxChar == '\n') || (rxChar == '\r')){ //check input on enter
	    // todo need to protect this part so we don't get context switching
	    received_buffer[buffer_position - 1] = '\0';
	    if (strcmp(received_buffer, "led on") == 0) gpio_put(16,1);
	    if (strcmp(received_buffer, "led off") == 0) gpio_put(16,0);
	    buffer_position = 0; 
	  }
	}
  uart_set_irq_enables(uart0, true, false); // enable receive interrupt only after processing command
  }}

// freertos demo of direct notification
void UART_Isr(){ // uart received a character
  uart_set_irq_enables(uart0, false, false); // disable uart interrupt so it doesn't fire again
  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(xTaskToNotify_UART, &xHigherPriorityTaskWoken); // let uart task knows there is received character
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);}

int main() {
    stdio_init_all();
     
    gpio_init(16);
    gpio_set_dir(16, 1);
    gpio_put(16, 0);
    
    //init uart
    gpio_set_function(0, GPIO_FUNC_UART); // set gpio 0 and 1 to uart functions
    gpio_set_function(1, GPIO_FUNC_UART);
    uart_set_baudrate(uart0, 115200);   
    uart_set_hw_flow(uart0, false, false); // no hardware flow control
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
    irq_set_exclusive_handler(UART0_IRQ, UART_Isr); // set up interrupt handler
    irq_set_enabled(UART0_IRQ, true);
    
    xTaskCreate(uart_task, "UART task", 1024 * 3, NULL, 2, &xTaskToNotify_UART);
	vTaskStartScheduler();
	for(;;){}
	return 0;}


