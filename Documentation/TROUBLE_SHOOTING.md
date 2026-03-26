# Pitfalls

## SMPLF X
Bei der Vereinfachung von sehr großen Ausdrücken (mehrere Millionen Nodes) kann es durch eine Ungünstige Wahl von Simplifizierungstemplates dazu kommen, dass der Simplifizierungsvorgang bei einem konkreten Simplifizierungsschritt X einfriert (Standardmäßig SMPLF 0 oder SMPLF 3). Behoben werden kann das in den meisten Fällen durch Umformulierung von Templates und Optimierung dieser für große Ausdrücke (zb. StdTemplate >> ArgTemplate)