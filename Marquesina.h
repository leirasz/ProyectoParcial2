/**
 * @file Marquesina.h
 * @brief Declaración de la clase Marquesina para animación de texto en consola tipo marquesina.
 *
 * Esta clase permite mostrar un texto desplazándose horizontalmente en la parte superior de la consola,
 * utilizando hilos, sincronización y manipulación avanzada de la consola de Windows.
 */
#ifndef MARQUESINA_H_INCLUDED
#define MARQUESINA_H_INCLUDED
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <chrono>
#include <vector>

/**
 * @class Marquesina
 * @brief Clase para animar un texto tipo marquesina en la consola de Windows.
 *
 * Gestiona la animación, sincronización y renderizado de un texto desplazándose horizontalmente
 * en la línea superior de la consola, usando hilos y buffers virtuales.
 */
class Marquesina {
public:
    /**
     * @brief Constructor. Inicializa la marquesina y configura la consola.
     */
    Marquesina();
    /**
     * @brief Destructor. Detiene la animación y libera recursos.
     */
    ~Marquesina();
    /**
     * @brief Inicia la animación de la marquesina con el texto proporcionado.
     * @param texto Texto a mostrar en la marquesina.
     */
    void iniciar(const std::string& texto);
    /**
     * @brief Detiene la animación de la marquesina y limpia la línea superior.
     */
    void detener();
    /**
     * @brief Pausa la animación de la marquesina.
     */
    void pausar();
    /**
     * @brief Reanuda la animación de la marquesina si estaba pausada.
     */
    void reanudar();
    /**
     * @brief Fuerza la actualización de la pantalla completa (usado por el menú).
     */
    void actualizar_pantalla_completa();
    
private:
    /**
     * @brief Indica si la animación está en ejecución.
     */
    std::atomic<bool> ejecutando;
    /**
     * @brief Indica si la animación está pausada.
     */
    std::atomic<bool> pausado;
    /**
     * @brief Indica si se requiere actualizar la pantalla.
     */
    std::atomic<bool> necesita_actualizacion;
    /**
     * @brief Hilo encargado de la animación de la marquesina.
     */
    std::thread hilo_marquesina;
    /**
     * @brief Texto que se muestra en la marquesina.
     */
    std::string texto_marquesina;
    /**
     * @brief Ancho actual de la consola.
     */
    int ancho_consola;
    /**
     * @brief Alto actual de la consola.
     */
    int alto_consola;
    /**
     * @brief Posición actual del texto en la marquesina.
     */
    int posicion_actual;
    /**
     * @brief Mutex para sincronizar el acceso a la consola.
     */
    mutable std::mutex mutex_consola;
    /**
     * @brief Buffer virtual de pantalla para la animación.
     */
    std::vector<std::string> buffer_pantalla;
    /**
     * @brief Buffer de colores para cada línea de la pantalla.
     */
    std::vector<WORD> buffer_colores;
    /**
     * @brief Manejador de la consola de Windows.
     */
    HANDLE handle_consola;
    /**
     * @brief Información original del buffer de la consola.
     */
    CONSOLE_SCREEN_BUFFER_INFO info_consola_original;
    /**
     * @brief Hilo principal que ejecuta la animación de la marquesina.
     */
    void ejecutar_marquesina();
    /**
     * @brief Obtiene las dimensiones actuales de la consola.
     */
    void obtener_dimensiones_consola();
    /**
     * @brief Inicializa los buffers de pantalla y color.
     */
    void inicializar_buffer();
    /**
     * @brief Actualiza el buffer de la línea de marquesina.
     */
    void actualizar_marquesina_en_buffer();
    /**
     * @brief Renderiza el buffer completo en la consola.
     */
    void renderizar_buffer_completo();
    /**
     * @brief Captura el estado actual de la consola.
     */
    void capturar_estado_consola();
    /**
     * @brief Restaura el estado original de la consola.
     */
    void restaurar_estado_consola();
    /**
     * @brief Escribe una línea en la consola de forma segura.
     * @param fila Número de fila donde escribir.
     * @param texto Texto a mostrar.
     * @param color Color de texto a usar.
     */
    void escribir_linea_segura(int fila, const std::string& texto, WORD color);
};

#endif