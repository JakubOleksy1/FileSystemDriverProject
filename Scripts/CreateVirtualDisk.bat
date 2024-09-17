@echo off

mkdir "C:\virtualdisk"
(
    echo create vdisk file="C:\virtualdisk\virtualdisk.vhdx" maximum=1024
    echo select vdisk file="C:\virtualdisk\virtualdisk.vhdx"
    echo attach vdisk
    echo convert gpt
    echo create partition primary
    echo format fs=ntfs quick
    echo assign letter=W
) > temp_diskpart_script.txt

diskpart /s temp_diskpart_script.txt

del temp_diskpart_script.txt