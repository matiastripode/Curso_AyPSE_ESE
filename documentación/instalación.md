# Instalación

Para las prácticas se utilizará la placa [ESP32-C6-DevKitC-1](https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide.html) como hardware y el software Visual Studio Code como entorno integrado de desarrollo (IDE).
Para que este último sea capaz de programar la placa de desarrollo es necesario descargar componentes de software adicionales.
Además, es necesario descargar la herramienta Git para trabajar con repositorios como lo haremos en el cursado.
A continuación se detallan los pasos necesarios para la instalación de estas herramientas:

1. [Git](#git)
2. [Visual Studio Code](#visual-studio-code)
3. [ESP IDF](#esp-idf)
4. [C/C++ Extension](#c/c++-extension)

## Git

Git es un software utilizado para llevar correctamente un registro de cambios en un conjunto de archivos, al mismo tiempo que permite el trabajo colaborativo entre programadores.
Para la instalación del paquete diríjase a la siguiente [página](https://git-scm.com/downloads) y seleccione la versión correspondiente a su sistema operativo.
En el caso de ser Windows descargue el instalador “Standalone” para la arquitectura de su ordenador.  
Ejecute el instalador, observará una ventana como en la siguiente figura:

![git](./imágenes/git.png)

Elija en cada ventana la opción por defecto y luego presione instalar.
En el caso de utilizar Linux, para la mayoría de las distribuciones basta con correr los siguientes comando en una terminal para instalar el paquete git:

```PowerShell
sudo apt-get update
sudo apt-get install git
```

## Visual Studio Code

Visual Studio Code es un IDE y editor de código desarrollado por Microsoft con soporte para múltiples lenguajes de programación.
Para descargarlo ingrese a la siguiete [página](https://code.visualstudio.com/) y elija la versión correspondiente a su sistema operativo.
Al ejecutar el instalador aparecerá la pantalla que se observa en la siguiente figura. Acepte las condiciones e instale seleccionando las opciones por defecto.

![vscode](./imágenes/vscode.png)

## ESP-IDF

ESP-IDF (Espressif IoT Development Framework) es la plataforma oficial de desarrollo de Espressif (el fabricante de la placa a utilizar) para los modulos ESP32.
Para su instalación siga los siguietes pasos:

1. Abra la aplicación Visual Studio Code y diríjase a la vista de Extensiones:  ![extensions](https://raw.githubusercontent.com/microsoft/vscode-icons/2ca0f3225c1ecd16537107f60f109317fcfc3eb0/icons/dark/extensions.svg)

2. Busque la extensión de ESP-IDF (use palabras clave como `espressif`, `esp-idf`, `esp32`, etc).

3. Presione el botón `Install`.

    ![idf1](./imágenes/idf1.png)

4. Una vez instalada, podrá ver un nuevo ícono en la barra de herramientas: ![idf2](./imágenes/esp-idf_logo.svg) ESP-IDF Explorer. Diríjase a esa vista y haga clic en `Advanced` -> `Open ESP-IDF Instalation Manager`.

    ![idf2](./imágenes/idf2.png)

5. Se abrirá la aplicación `ESP-IDF Instalation Manager`. Hacer click en  `Start Installation`.

    ![idf3](./imágenes/idf3.png)

6. En el menú siguiente seleccionar la opción `Start Easy Installation`.

    ![idf4](./imágenes/idf4.png)

7. A continuación se le mostrarán la información de la instalación:
   - Versión: `v6.0.1` o superior.
   - Directorio de ESP-IDF: por defecto `%USERPROFILE%\esp`
   - Tamaño estimado:  son necesarios al menos `3.5GB`.

    Una vez verificada la información, haga clic en `Start Installation`.

    ![idf5](./imágenes/idf5.png)

> [!IMPORTANT]
> Asegúrese que los path de la carpeta de instalación no contenga `espacio`, la letra `ñ` o letras con tilde `á`, `é`, `í`, `ó`, `ú`.

8. La instalación demorará varios minutos. Una vez completada podrá observar un mensaje indicando la instalación ha sido completada. Ya puede cerrar la ventana del instalador.

    ![idf6](./imágenes/idf6.png)
    ![idf6](./imágenes/idf7.png)

## C/C++ Extension

La extensión de Microsoft para C y C++ agrega soporte de lenguaje para C/C++ a Visual Studio Code, incluidas funciones de edición y depuración.

1. Abra la aplicación Visual Studio Code y diríjase a la vista de Extensiones:  ![extensions](https://raw.githubusercontent.com/microsoft/vscode-icons/2ca0f3225c1ecd16537107f60f109317fcfc3eb0/icons/dark/extensions.svg)

2. Busque la extensión de C/C++.

3. Presione el botón `Install`.

    ![c-cpp](./imágenes/c-cpp.png)

---

Una vez finalizada la instalación de las herramientas puede continuar con el instructivo de [Creación de Repositorio](./repositorio.md)
