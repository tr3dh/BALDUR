<!-- Sprachumschalter -->
<p align="right">
🌐 <a href="./README.md">English</a> | <a href="./README.de.md">Deutsch</a>
</p>

# Basic Albebraic Language for Deriving Unified Tensor Representation (BALDUR)

<img align="left" style="width:360px" src="Recc/textures/Baldur_Logo_whiteBG.svg">

Baldur ist eine minimalistische Interpretersprache, die das symbolische Arbeiten mit algebraischen und index-notierten Tensorausdrücken nativ unterstützt. Das Programm implementiert dabei ein C++ Tensoralgebrabackend und schaltet dieses über eine mit der Intepreterschmiede [Alberich](https://github.com/tr3dh/ALBERICH) erzeugte Sprachdistribution frei. Die Skriptsprache Baldur wurde im Rahmen einer [Masterarbeit](Recc/Thesis/Masterthesis_Rother_2026_public.pdf) für das Institut für Kontinuumsmechanik (IKM) der Leibniz Universität Hannover (LUH) entwickelt.

# 🧬 Entwicklung

- Sprach Standard: C++20
- Compiler: gcc/g++
- Subsystem: MinGw64
- Platform: Windows
- Buildsystem: Make

# 🔧 Funktionsumfang

- Assemblieren symbolischer algebraischer und index-notierter Ausdrücke
- algebraische Ableitung, Vereinfachung, Substitution
- Ergebnis-Export der Indexnotation als Julia-Skript

# 🚚 Lieferumfang

- Interpreter
- Language Server
- VSCode Extension (packt Interpreter + Language Server)
- minimalistischer Editor (kein Language Support)
- Source Code

Die Builds können über die [Releases](https://github.com/tr3dh/BALDUR/releases) heruntergeladen werden.

# 🛠️ Sprachcharakteristiken

Die primäre Anwendung Baldurs ist die symbolische Tensorrechnung mit umfangreichen Tensorausdrücken. Das Backend kann bei adäquater Nutzung mit sehr umfangreichen Ausdrücken innerhalb moderater Laufzeiten rechnen (getestet bis 110 mio Nodes, läuft 3.5 h). Aufgrund der algebraischen Eingaben orientiert sich die Sprachsyntax stark an der gebräuchlichen Tensoralgebra-Schreibweise. Das freigeschaltete Backend ist darauf ausgelegt dem Nutzer maximale Kontrolle einzurichten. Damit können anwendungsfallspezifische, sehr tiefgreifende Optimierungen vorgenommen werden. Der Nutzer kann beispielsweise die AST-Konvention der gebauten Ausdrücke steuern, sowie umfangreiche Regelwerke für Ableitungen und Vereinfachungen hinterlegen. Eine Plug-and-Play Lösung kann über die Includes [pckg](Examples/pckg/) eingerichtet werden. Zudem nutzt Baldur bzw. Alberich implizite und explizite, alternative Semantikkonzepte, die entweder Low-Level- oder High-Level-Kontrolle über die verwendeten Laufzeitressourcen bieten. So kann der Nutzer je nach Bedarf und Anwendungsfall die Speicherverwaltung und Objekthandhabung manuell einrichten oder dem Interpreter überlassen. Zudem implemnentiert Baldur eine dynamisch konfigurierbare Überladungskonfiguration. Dabei können der verwendete Operatorensatz, die Operatorenhierarchie und die Operatorenüberladung über die `__LPECONFIG.JSON` konfiguriert werden. Neben den genannten Sprachcharakteristiken implementiert die Grundsprache weitere interessante Syntaxkonzepte wie Simultanverknüpfungen, input-verschaltende if-Konventionen, eine vollständig rekursionfähige Syntax und mehr. Weitere Information über die dort implementierten Syntaxkonzepte können im GitHub Repository der Interpreterschmiede [Alberich](https://github.com/tr3dh/ALBERICH) und der hinterlegten [Masterarbeit](Recc/Thesis/Masterthesis_Rother_2026_public.pdf) eingesehen werden.

# ✒️ Language Surpport

Der Language Support der Sprache Baldur kommt mit folgenden Providern:
- Autovervollständigung
- Hover
- dynamisches Syntaxhighlighting
- Goto Definition
- Find References

# 🚀 Erste Schritte

Die mitgelieferte [Masterarbeit](Recc/Thesis/Masterthesis_Rother_2026_public.pdf) bietet anhand von anschaulichen Beispielen und Referenzen der Grundsprache Alberich und der Distribution Baldur einen unkomplizierten Einstieg. Die [Sprachbeispiele](Examples/) stellen ebenfalls eine geeignete Einarbeitungsgrundlage dar. Der Language Support kann ebenfalls herangezogen werden, um implementierte Funktionen, Operatoren, Variablen und mehr zu untersuchen.

# 🪖 vorgesehener Ablauf

Der vorgesehene Ablauf wird durch das folgende, exemplarische Baldurskript beschrieben. Die zentrale Berechnung findet dabei vollständig in algebraischer Notation statt. Die Index-Notation ist hier lediglich ein Exportformat der algebraischen Notation und ist in erster Linie darauf ausgelegt aus einem algebraischen Ausdruck generiert zu werden.

```Cpp
// Erzeugen Abhängigkeiten
int nComps = 3

// Funktion tExpr(label, order, dimensions) gibt einen externen, algebraischen Knoten zurück
A, B = tExpr("A", 2, args(nComps, nComps)), tExpr("B", 2, args(nComps, nComps))
a, b = tExpr("a", 0), tExpr("b", 0)

// algebraische Assemblierung
expr = wrap(a * A + b * B)

// Ableitung, Vereinfachung, Substitution, anderweitige Manipulation, ...
// ...  

// Umwandlung in Indexnotation
assert(expr->isUnwrapped() == getUnwrapOperands(), "...")
tIdn idnExpr = toIDN(expr)

// Export zu Julia-Skript
string jlScript = idnExpr->toJuliaString("evalExpr")
jlScript->writeToFile("../Examples/juliaScripts/expr.jl")

slog(jlScript)
// Ausgabe : '
// ...
// function evalExpr(a, A, b, B)
//  ...
// 	@tensor opt=true res[idx2, idx3] := ((a * A[idx2, idx3]) + (b * B[idx2, idx3]))
// 	return res
// end
// '
```

# 🤝 Danksagung

Mein besonderer Dank gilt meinem Betreuer Dr. Hendrik Geisler, der während der Entwicklung eine große Hilfe war.

Hendrik Geisler wurde während der Betreuungszeit von der Europäischen Union (ERC, Gen-TSM, project number 101124463) finanziert. Die geäußerten Ansichten und Meinungen sind jedoch ausschließlich die des Autors/der Autoren und spiegeln nicht unbedingt die der Europäischen Union oder der Exekutivagentur des Europäischen Forschungsrats wider. Weder die Europäische Union noch die Bewilligungsbehörde können für sie verantwortlich gemacht werden.

## 📚 Verwendete Bibliotheken und Assets
Bedanken möchte ich mich zudem bei den jeweiligen Entwicklern und Maintainern der im Rahmen des Projekts verwendeten Open-Source Bibliotheken und Designern der im Rahmen des Projekts verwendeten Open-Source Assets.
Diese sind im Folgenden aufgeführt. Die zugehörigen Lizenztexte sind im Ordner [thirdPartyLicenses](/thirdPartyLicenses/) hinterlegt.

| Library / Asset       | License                          |
|-------------------|----------------------------------|
| [raylib](https://www.raylib.com/)             | zlib/libpng                      |
| [Eigen](https://eigen.tuxfamily.org/)         | MPL2 (Mozilla Public License 2.0) |
| [SymEngine](https://github.com/symengine/symengine) | BSD 2-Clause                     |
| [magic_enum](https://github.com/Neargye/magic_enum) | MIT                              |
| [nlohmann/json](https://github.com/nlohmann/json)   | MIT                              |
| [Boost.PFR](https://github.com/boostorg/pfr)        | Boost Software License 1.0       |
| [Dear ImGui](https://github.com/ocornut/imgui)      | MIT                              |
| [rlImGui](https://github.com/raylib-extras/rlImGui) | MIT                              |
| [ImPlot](https://github.com/epezent/implot)         | MIT                              |
| [ImGuiFileBrowser](https://github.com/AirGuanZ/imgui-filebrowser) | MIT                   |
| [lsp-framework](https://github.com/leon-bckl/lsp-framework) | MIT                            |
| [LLVM](https://github.com/llvm/llvm-project) |    Apache 2.0 with LLVM Exeptions |
| [angle](https://github.com/google/angle) | ANGLE |
| [Anvil Icon (Alberich Logo)](https://www.svgrepo.com/svg/308950/anvil-hard-metal-metalworking-tool)   | [CC0](https://creativecommons.org/publicdomain/zero/1.0/legalcode.txt) |
| [Spear and Shield Icon (Baldur Logo)](https://www.svgrepo.com/svg/37858/spear)                        | [CC0](https://creativecommons.org/publicdomain/zero/1.0/legalcode.txt) |
| [Julimono Font](https://github.com/cormullion/juliamono)                          | OFL-1.1 |