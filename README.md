# Computer-Graphic-Project

## Instalacja bibliotek

Projekt korzysta z bibliotek instalowanych przez `vcpkg`.

`vcpkg` to menedżer bibliotek C/C++, który pobiera wymagane biblioteki i automatycznie podłącza je do Visual Studio.

Wymagane biblioteki są zapisane w pliku:

```text
vcpkg.json
```

---

## Instalacja vcpkg

Otwórz PowerShell i wpisz:

```powershell
cd C:\Users\%USERNAME%\Desktop
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
```

Po wykonaniu tych komend `vcpkg` będzie zainstalowany i podłączony do Visual Studio.

---

## Instalacja bibliotek z vcpkg.json

Przejdź do głównego folderu projektu, czyli tam, gdzie znajduje się plik `vcpkg.json`:

```powershell
cd C:\Users\%USERNAME%\Desktop\Computer-Graphic-Project
```

Następnie uruchom:

```powershell
C:\Users\%USERNAME%\Desktop\vcpkg\vcpkg.exe install --triplet x64-windows
```

`vcpkg` odczyta plik `vcpkg.json` i pobierze wszystkie wymagane biblioteki.

---

## Uruchamianie projektu

Otwórz projekt przez plik:

```text
Computer-Graphic-Project.sln
```

W Visual Studio ustaw konfigurację:

```text
Debug | x64
```


## Struktura projektu

Under-Water-World/
│
├── src/
│   ├── main.cpp
│   │
│   ├── core/
│   │   ├── Application.cpp
│   │   ├── Window.cpp
│   │   ├── Input.cpp
│   │   ├── Camera.cpp
│   │   └── Time.cpp
│   │
│   ├── graphics/
│   │   ├── Shader.cpp
│   │   ├── Render_Utiles.cpp
│   │   └── Renderer.cpp
│   │   
│   │   
│   │  
│   │   
│   │
│   ├── scene/
│   │   ├── Scene.cpp
│   │   └── Fish.cpp
│   │   
│   │
│   └── ui/
│       
│
├── include/
│   ├── core/
│   │   ├── Application.h
│   │   ├── Window.h
│   │   ├── Input.h
│   │   ├── Camera.h
│   │   └── Time.h
│   │
│   ├── graphics/
│   │   ├── Shader.h  
|   |   ├── Render_Utiles.cpp
│   │   └── Renderer.h
│   │   
│   │   
│   │   
│   │   
│   │   
│   │
│   ├── scene/
│   │   ├── Scene.h
│   │   └── Fish.h
│   │   
│   │
│   └── ui/
│       
│
├── shaders/
│   ├── basic.vert
│   ├── basic.frag
│   ├── fish.vert
│   └── fish.frag
│
└── assets/
    ├── models/
    ├── textures/
    └── skybox/