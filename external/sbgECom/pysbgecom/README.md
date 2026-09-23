# pysbgecom

`pysbgecom` is a Python library to interface SBG Systems IMU, AHRS, and INS products.
The project is platform and OS independent.

## Repository Contents

- Core library source code in `sbgecom/`
- Command line tools:
  - `sbg_basic_logger`
  - `sbgecom_api`
- Usage examples in `examples/`
- Unit tests in `tests/`

## Requirements

- Python `>= 3.11.2`
- `pip`

## Installation

### Install from a local clone

```sh
python -m pip install -e .
```

### Install as a regular package

```sh
python -m pip install .
```

After installation, the following CLI entrypoints are available:

- `sbg-basic-logger`
- `sbgecom-api`
- `sbgecom-example`

## Quick Start (Library)

```python
from sbgecom import SbgEComHandle, SbgInterfaceSerial

serial_interface = SbgInterfaceSerial(port="/dev/ttyUSB0", baudrate=921600)
handle = SbgEComHandle(serial_interface)

try:
    while True:
        handle.handle_one_log()
except KeyboardInterrupt:
    pass
finally:
    handle.close()
```

## Tools

### sbg_basic_logger

`sbg_basic_logger` logs and displays sbgECom binary messages as text.

Run from the repository root:

```sh
python -m tools.sbg_basic_logger.src.main --help
```

Run after package installation:

```sh
sbg-basic-logger --help
```

Serial example (console only):

```sh
python -m tools.sbg_basic_logger.src.main -s <SERIAL-PORT> -r <BAUDRATE> -p
```

Serial example (console + files):

```sh
python -m tools.sbg_basic_logger.src.main -s <SERIAL-PORT> -r <BAUDRATE> -p -w --dir <OUTPUT_DIR>
```

UDP example (console only):

```sh
python -m tools.sbg_basic_logger.src.main -a <IP_ADDR> -I <UDP_PORT_IN> -O <UDP_PORT_OUT> -p
```

Input file example (console only):

```sh
python -m tools.sbg_basic_logger.src.main -i <BINARY_FILE> -p
```

### sbgecom_api

`sbgecom_api` reads and updates product configuration through the JSON REST API over serial or UDP.

Run from the repository root:

```sh
python -m tools.sbgecom_api.src.main --help
```

Run after package installation:

```sh
sbgecom-api --help
```

Read configuration (GET):

```sh
python -m tools.sbgecom_api.src.main -s COM5 -r 921600 api/v1/settings/ports/comA -g
```

Update configuration (POST):

```sh
python -m tools.sbgecom_api.src.main -s COM5 -r 921600 api/v1/settings/ports/comB/baudrate -p -b 230400
```

UDP access example:

```sh
python -m tools.sbgecom_api.src.main -a 10.10.0.12 -I 52141 -O 52140 api/v1/settings -g
```

## Examples

Run from the repository root:

```sh
python -m examples.main --help
```

Run after package installation:

```sh
sbgecom-example --help
```

Serial example:

```sh
python -m examples.main --interface serial --port <SERIAL-PORT> --baudrate <BAUDRATE>
```

UDP example:

```sh
python -m examples.main --interface udp --remote-ip <IP_ADDR> --remote-port <UDP_PORT> --listen-port <UDP_PORT>
```

Input file example:

```sh
python -m examples.main --interface file --file <BINARY_FILE>
```

## Tests

Run all tests:

```sh
pytest .
```

Run one test file:

```sh
pytest tests/logs_tests/sbgecom_log_gnss_pos_test.py
```

## Documentation (Sphinx)

```sh
python -m pip install sphinx
mkdir -p doc
cd doc
sphinx-quickstart
sphinx-apidoc -o source ../
make html
```

Generated HTML documentation is available in `doc/build/html/`.

## License

This project is released under the MIT License. See `LICENSE`.
