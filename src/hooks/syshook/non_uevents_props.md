### Power Supply
- `capacity`
- `energy_now`
- `charge_now`
- `power_now`
- `current_now`
- `voltage_now`
- `temp`
- `cycle_count`
- `capacity_level`

### Hardware Monitoring (`/sys/class/hwmon/*`)
- `temp*_input`
- `fan*_input`
- `in*_input`
- `curr*_input`
- `power*_input`

### CPU Frequency (`/sys/devices/system/cpu/cpu*/cpufreq/`)
- `scaling_cur_freq`
- `cpuinfo_cur_freq`
- `cpuinfo_avg_freq`

### CPU Statistics (`/proc/stat`)
- `user`
- `system`
- `idle`
- `iowait`
- `irq`
- `softirq`

### Block Device Statistics (`/sys/block/*/stat`)
- `reads_completed`
- `writes_completed`
- `sectors_read`
- `sectors_written`
- `io_time`

### Network Interface Statistics (`/sys/class/net/*/statistics`)
- `rx_bytes`
- `tx_bytes`
- `rx_packets`
- `tx_packets`
- `rx_errors`
- `tx_errors`
- `collisions`

### Memory Information (`/proc/meminfo`)
- `MemAvailable`
- `MemFree`
- `Buffers`
- `Cached`
- `Active`
- `Inactive`

### GPU Telemetry (driver-dependent `/sys/class/hwmon/*` or `/sys/class/drm/*`)
- `temperature`
- `power`
- `clock_speeds`
