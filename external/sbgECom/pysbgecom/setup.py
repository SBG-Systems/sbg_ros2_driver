from pathlib import Path

from setuptools import find_packages, setup

project_root = Path(__file__).resolve().parent
requirements = (project_root / "requirements.txt").read_text(encoding="utf-8").splitlines()
long_description = (project_root / "README.md").read_text(encoding="utf-8")

setup(
	name="pysbgecom",
	version="6.0",
	author="SBG Systems",
	description="Python library used to interface SBG Systems IMU, AHRS and INS products.",
	long_description=long_description,
	long_description_content_type="text/markdown",
	license="MIT",
	packages=find_packages(
		include=[
			"sbgecom*",
			"tools*",
			"examples*",
		]
	),
	install_requires=requirements,
	python_requires=">=3.11.2",
	entry_points={
		"console_scripts": [
			"sbg-basic-logger=tools.sbg_basic_logger.src.main:main",
			"sbgecom-api=tools.sbgecom_api.src.main:main",
			"sbgecom-example=examples.main:main",
		],
	},
)
