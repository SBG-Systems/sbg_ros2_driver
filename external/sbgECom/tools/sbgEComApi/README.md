# sbgEComApi

The sbgEComApi command line tool let you read/update your product configuration through the JSON REST API.
A typical REST API works using GET / POST requests other HTTP protocols.

The sbgEComApi tool encapsulate GET/POST requests so it can be used over a serial or UDP interfaces.
This tool is perfect to create complex and automated configuration scripts with no code.

# Usage

The sbgEComApi implements a simple to use command line interface (CLI) with two main operations.

## Read Configuration (GET)

You can read the whole product configuration or a specific configuration node.
The following example, reads the Port A configuration.
It uses the COM5 at 921600 bps to communicate with the device:

```sh
sbgEComApi.exe -s COM5 -r 921600 /api/v1/settings/ports/comA -g
```

You should get the following response:
```js
{"mode":"rs232", "parity":"none", "baudrate":921600}
```

## Update Configuration (POST)

You can update the whole product configuration at once or each setting one by one.
In the following example, we just update the Port A baudrate to 230400 bps.
The tool uses the computer COM5 at 921600 bps to communicate with the device:

```sh
sbgEComApi.exe -s COM5 -r 921600 /api/v1/settings/ports/comA/baudrate -p -b 230400
```

You should get the following response:
```js
{"title":"request successful","needReboot":false}
```

## UDP access

High Performance INS support sbgECom commands only on the Ethernet interface 0 and on a
default internal interface.

This default internal interface can't be configured and always use the same settings.
It listen on the port 52140 and respond on port 52141.

The example below shows how to retrieve the device settings using this default interface:

```sh
sbgEComApi.exe -a 10.10.0.12 -I 52141 -O 52140 /api/v1/settings -g
```

# Options
You can access the tool help using the --help argument.

```
Usage: sbgEComApi [-gpS] [--help] [--version] [-a IP address] [-I UDP port in] [-O UDP port out] [-s SERIAL_DEVICE] [-r SERIAL_BAUDRATE] [-n NR_ATTEMPTS] [-t TIMEOUT] [-q QUERY] [-b BODY] [-B BODY_FILE] [-o OUTPUT_FILE] PATH

Access a RESTful SBG ECom server.

    Serial example: sbgEComApi -s <SERIAL-PORT> -r <BAUDRATE> api/v1/settings -g
    UDP example:    sbgEComApi -a <IP_ADDR> -I <UDP_PORT_IN> -O <UDP_PORT_OUT> api/v1/settings -g

  --help                                             display this help and exit
  --version                                          display version info and exit
  -a, --addr-ip=IP address                           open an UDP interface
  -I, --udp-port-in=UDP port in                      UDP port to receive data from (local)
  -O, --udp-port-out=UDP port out                    UDP port to send data to (remote)
  -s, --serial-device=SERIAL_DEVICE                  open a serial interface
  -r, --serial-baudrate=SERIAL_BAUDRATE              serial baudrate
  -n, --nr-attempts=NR_ATTEMPTS                      number of transaction attempts
  -t, --timeout=TIMEOUT                              reply time-out, in seconds
  -g, --method-get                                   use the GET method (default)
  -p, --method-post                                  use the POST method
  -q, --query=QUERY                                  query string, format=pretty&delta=true, format and delta options are optional
  -b, --body=BODY                                    body (POST method only)
  -B, --body-file=BODY_FILE                          file containing the body (POST method only)
  -S, --print-status                                 print the status code on the output stream
  -o, --output-file=OUTPUT_FILE                      output file
  PATH                                               GET or POST request path endpoint

BODY or BODY_FILE may only be provided when using the POST method.

If provided, BODY_FILE may not contain binary data.

PATH is a URI path component such as api/v1/settings

Exit codes :
    0: 200 OK
   60: 400 BAD REQUEST
   61: 401 UNAUTHORIZED
   63: 403 FORBIDDEN
   64: 404 NOT FOUND
   69: 409 CONFLICT
   82: 422 UNPROCESSABLE ENTITY
  100: 200 INTERNAL SERVER ERROR

EXIT_FAILURE for a general error unrelated to the status code or if the status code is unknown.
```