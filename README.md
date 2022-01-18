# Demo For ODB+Sqlite3+Qt+QQuick

# Author
	eton

# Version
	- 22/02/11 generate file-odb.{h|i|c|}xx by odb cli.
	- 22/02/12 generate file-odb.{h|i|c|}xx by odb-cmake auto.
# Usage:

- generate csv file from excel, put all input csv file to below path, then run application, the sqlite db file named "apai-gen.db" will in same directory.
	
```
	/tmp/MTPs.csv       /tmp/UTPs.csv          /tmp/modes.csv   /tmp/pulses.csv
	/tmp/UTP_Infos.csv  /tmp/apodizations.csv  /tmp/probes.csv
```

- export csv from Excel:

```
Sub ExportSheetsToCSV()
	Dim xWs As Worksheet
	Dim xcsvFile As String
	For Each xWs In Application.ActiveWorkbook.Worksheets
		xWs.Copy
		xcsvFile = CurDir & "\" & xWs.Name & ".csv"
		Application.ActiveWorkbook.SaveAs Filename: = xcsvFile, _
		FileFormat: = xlCSV, CreateBackup: = False
		Application.ActiveWorkbook.Saved = True
		Application.ActiveWorkbook.Close
	Next
End Sub

```
# refs:
- https://github.com/BtbN/OdbCmake.git
