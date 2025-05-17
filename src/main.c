#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/uart.h>

// use the macro call DEVICE_DT_GET() to get info about device
const struct device *uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));

// Define UART configuration
const struct uart_config uart_cfg = {
    .baudrate = 115200,
    .parity = UART_CFG_PARITY_NONE,
    .stop_bits = UART_CFG_STOP_BITS_1,
    .data_bits = UART_CFG_DATA_BITS_8,
    .flow_ctrl = UART_CFG_FLOW_CTRL_NONE
};

//define a transmission buffer to hold data sent
static uint8_t tx_buff[20];  // increased buffer size

//uart call back function
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    switch (evt->type)
    {
    case UART_TX_DONE:
        printk("TRANSMITTING DONE\n");
        break;
    case UART_TX_ABORTED:
        printk("TRANSMITTING ABORTED\n");
        break;
    
    default:
        break;
    }
}

int main()
{
    int cnt = 0;
    if(!device_is_ready(uart1)){
        printk("UART1 is not ready \n\r");
        return 1;
    }
    
    // configure uart
    int err = uart_configure(uart1, &uart_cfg);
    if (err == -ENOSYS)
    {
        return -ENOSYS;
    }
    
    // Register the callback function
    err = uart_callback_set(uart1, uart_cb, NULL);
    if (err) {
        printk("Failed to set callback: %d\n", err);
        return err;
    }

    while(cnt <= 2)
{
    
    snprintf(tx_buff, sizeof(tx_buff), "%d\n", cnt);
    
    /* transmit data */
    err = uart_tx(uart1, tx_buff, strlen(tx_buff), SYS_FOREVER_US);
    if (err)
    {
        printk("Failed to send data: %d\n", err);
        return err;
    }
    
    cnt++;
    if (cnt > 2)
    {
        cnt = 0;
    }
    
    k_msleep(5000);
}

    
    k_msleep(100);
    return 0;
}