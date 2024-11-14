100 ' INKEY$ sample
105 IF INKEY$<>"" GOTO 105
110 PRINT "Enter any key to end.."
120 A$=INKEY$
130 IF A$="" THEN 110 ELSE END
