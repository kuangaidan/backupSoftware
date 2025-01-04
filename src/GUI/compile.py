from cx_Freeze import setup, Executable

setup(
    name="Backup Software",
    version="0.9",
    description="Generic Data Backup Software",
    executables=[Executable("src/GUI/start.py")]
)
