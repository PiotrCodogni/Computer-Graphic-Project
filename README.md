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

## Ręczna instalcja bibliotek z vcpkg.json

Przejdź do głównego folderu projektu, czyli tam, gdzie znajduje się plik `vcpkg.json`:
Następnie uruchom:

```powershell
C:\Users\%USERNAME%\Desktop\vcpkg\vcpkg.exe install --triplet x64-windows
```

`vcpkg` odczyta plik `vcpkg.json` i pobierze wszystkie wymagane biblioteki.

---

## Automatyczna instalcja bibliotek
W projekcie powinny być włączone opcje:
Use Vcpkg: Yes
Use Vcpkg Manifest: Yes
Install Vcpkg Dependencies: Yes
Use AutoLink: Yes
App-locally deploy DLLs: Yes

Można je sprawdzić w:
Project Properties
→ Configuration Properties
→ vcpkg

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

 ---

## Struktura projektu

Computer-Graphic-Project/
│
├── Computer-Graphic-Project.sln
├── vcpkg.json
├── README.md
│
└── Under-Water-World/
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
    │   │   ├── Texture.cpp
    │   │   ├── Model.cpp
    │   │   ├── Renderer.cpp
    │   │   └── Render_Utils.cpp
    │   │
    │   └── scene/
    │       ├── Scene.cpp
    │       └── entity/
    |           └──Fish1.cpp
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
    │   │   ├── Texture.h
    │   │   ├── Model.h
    │   │   ├── Renderer.h
    │   │   └── Render_Utils.h
    │   │
    │   └── scene/
    │       ├── Scene.h
    │       └── entity/
    |           └──Fish1.h
    │
    ├── shaders/
    │   ├── basic.vert
    │   ├── basic.frag
    │   ├── fish.vert
    │   └── fish.frag
    │
    └── assets/
        └── models/
        |    └── fish1.glb
        |        
        |
        ├─skybox/
        |
        └──texture/
            └── fish1.png     

## Opis modułów
main.cpp
Punkt startowy programu; tworzy obiekt Application, uruchamia inicjalizację, główną pętlę aplikacji i sprzątanie zasobów.

core/Application
Główna klasa aplikacji; zarządza oknem, czasem, inputem, sceną, rendererem oraz panelem ImGui.

core/Window
Odpowiada za utworzenie okna GLFW, obsługę zamknięcia aplikacji, wymianę buforów i pobieranie zdarzeń systemowych.

core/Input
Odpowiada za sprawdzanie, które klawisze są aktualnie wciśnięte.

core/Camera
Kamera podążająca za rybą; tworzy macierze view i projection używane podczas renderowania.

core/Time
Oblicza deltaTime, czyli czas pomiędzy klatkami, żeby ruch obiektów był niezależny od liczby FPS.

graphics/Shader
Wczytuje pliki shaderów, kompiluje vertex shader i fragment shader oraz tworzy program OpenGL.

graphics/Texture
Wczytuje tekstury z plików graficznych za pomocą stb_image i tworzy teksturę OpenGL.

graphics/Model
Wczytuje model 3D za pomocą Assimp i przekazuje jego dane do RenderContext.

graphics/Render_Utils
Tworzy i obsługuje bufory OpenGL, takie jak VAO, VBO i EBO, oraz rysuje geometrię modelu.

graphics/Renderer
Czyści ekran, pobiera kamerę ze sceny i wywołuje renderowanie obiektów sceny.

scene/Scene
Przechowuje obiekty znajdujące się w scenie, aktualizuje je i udostępnia je rendererowi.

scene/entity/Fish1
Reprezentuje rybę w scenie; przechowuje jej pozycję, obsługuje ruch, ładuje model i teksturę oraz renderuje obiekt.

shaders
Zawiera programy GLSL używane przez OpenGL do renderowania modeli.

assets
Zawiera pliki zasobów projektu, takie jak modele 3D i tekstury.


## Flow dziłania projektu

Application::run()
    ↓
Time::update()
    ↓
Input::update()
    ↓
Application::processInput()
    ↓
Scene::update()
    ↓
Fish::update()
    ↓
Camera::followTarget()
    ↓
Renderer::beginFrame()
    ↓
Renderer::render(scene)
    ↓
Fish::render()
    ↓
Texture::bind()
    ↓
Model::draw()
    ↓s
ImGui render
    ↓
Window::swapBuffers()
    ↓
gotowa klatka na ekranie