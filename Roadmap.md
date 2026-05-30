# Projekt
- Auf CMake umstellen (Cpp-CMake-Devstack Starter) 
- Doxygen Doku
- Fehlerausgaben vervollständigen und auf Englisch umstellen (Returning Asserts)

# Alberich
- Freischaltungen
    * ByteSequences (Serialisierungs Interaktionen für beliebige Klassen)
    * Scopes
    * json-data (nlohmann::json)
    * overflowsafe Integers and Doubles
- saubere Delete-Implementierung
    * Ermöglichen durch Abspeichern der LValue Positon (Iterator) im EvalRes
- sauberes Dref-Implementierung
    * Ermöglichen durch Abspeichern der Positon der referenzierten Variable (Iterator) in ReferenzVariablen
- StringID basierte Scopes (key)
- Args Übergabe per CMD, die vom Interpreter an Skripte durchgereicht wird

# Tensoralgebra Backend
- Weitere Exportformate
    * json
    * ltex
    * C/C++
    * ...
- Info Files für Laufzeitinformationsabfrage von Backendfunktionalitäten
    * Dummy Funktionsdeklarationen mit Dokstrings
    * Anschließender Backendfetch überschreibt diese Funktionalitäten dann
- StringID basierte Labels
- Über ECS gemanagete tExprs / tIdns
- Substitution in algebraische (Eigen) und indexnotierte Ausdrücke (rekursive Summen Eval, ComputeShader / -kernel)
- Interaktion mit externen Zeitschrittsimulationen
    * Abspeichern der Tangenten / Ausdrücken per ByteSequences im Evaluierskript (oder args gesteuerte Funktionalitätsunterscheidung)
    * Interaktionsskript (oder args gesteuerte Funktionalitätsunterscheidung) lädt Ausdrücke, substituiert Nutzer Eingaben
      (JSON, TXT, Pipeline-Input, ...) und gibt Ergebnis zurück (JSON, TXT, Pipiline-Output, ...)
- [jlFunc, Container] Map für dynamische Implementierung neuer Algebra-Container
- Umstieg auf Extensible Enums (UINT / isolierte StringID Wrapper) für dynamische Erweiterbarkeit

# Erweiterung
- Bereitstellen und Freischalten eines High-Level Interfaces für numerische Simulationsmethoden (FEM, FD, ...)