#!/bin/awk -f

function get_pair(str) {
  i = match(str, "=")
  property = substr(str, 0, i - 1)
  value = substr(str, i + 1)
}

/^UDEV +\[[[:digit:]]+\.[[:digit:]]+\] +add +.*usb.* \(block\)$/ {
  usb_block = 1
}

/^UDEV +\[[[:digit:]]+\.[[:digit:]]+\] +add +.*input\/input[[:digit:]]+ \(input\)$/ {
  input = 1
}

/^DEVNAME=.*$/ {
  get_pair($0)
  devname = value
}

/^DEVTYPE=partition$/ {
  usb_block_partition = usb_block
}

/^ID_FS_LABEL=.*$/ {
  get_pair($0)
  label = value
}

/^ID_MODEL=.*$/ {
  get_pair($0)
  model = value
}

/^NAME=.*$/ {
  get_pair($0)
  name = value
  sub("^\"", "", name)
  sub("\"$", "", name)
}

/^$/ {
  if (usb_block_partition) {
    cmd = "notify-send '" label " " model "' 'new device added " devname "'"
    system(cmd)
    close(cmd)
  } else if (input) {
    cmd = "notify-send '" name "' 'new input added'"
    system(cmd)
    close(cmd)
  }

  usb_block = 0
  usb_block_partition = 0
  input = 0
}
