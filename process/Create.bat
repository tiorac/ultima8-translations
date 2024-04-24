cd 03_CALC
U8CALC.exe -a
for %%f in (*.txt.new) do (
    move /Y "%%f" "..\04_COMP"
)

cd ..

cd 04_COMP
U8COMP.exe -a
for %%f in (*.cls.new) do (
    move /Y  "%%f" "..\05_EUSECODE_NEW"
)

cd ..

cd 05_EUSECODE_NEW
U8FT.exe -c EUSECODE.FLX.new
move /Y EUSECODE.FLX.new ..\EUSECODE.FLX