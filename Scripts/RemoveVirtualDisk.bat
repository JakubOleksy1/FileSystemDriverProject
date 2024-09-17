@echo off

(
    echo select vdisk file="C:\virtualdisk\virtualdisk.vhdx"
    echo detach vdisk
) > temp_diskpart_script.txt

diskpart /s temp_diskpart_script.txt

del temp_diskpart_script.txt

del C:\virtualdisk\virtualdisk.vhdx
rmdir C:\virtualdisk