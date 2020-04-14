Модуль проверялся на "Linux arch 5.6.3-arch1-1 #1 SMP PREEMPT Wed, 08 Apr 2020 07:47:16 +0000 x86_64 GNU/Linux"<br>
Компиляция:<br>
$ su<br>
\# make<br>
Тестирование:<br>
\# dmsetup create zero --table "0 1000 zero"<br>
\# dmsetup create dmp --table "0 1000 dmp /dev/mapper/zero"<br>
\# dd if=/dev/random of=/dev/mapper/dmp<br>
\# dd if=/dev/mapper/dmp of=/dev/null<br>
\# cat /sys/stat/volumes <br>
