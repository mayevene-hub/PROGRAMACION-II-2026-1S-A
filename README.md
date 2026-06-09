# PROGRAMACION-II-2026-1S-A

En este repositorio documentaré los avances sobre mi proyecto final, del juego para Programación-II.

# EXPLICACIÓN:

El juego será un roguelike 2D donde tendrás que desplazarte entre habitaciones las cuales necesitaran llaves, las cuales serán encontradas a traves del mapa, el juego tendrá una banda sonora, con el propósito de que los obstáculos, ya sean npcs o enemigos, se muevan al ritmo de la canción (utilizando loops en el sonido, utilizando delays para esto, para evitar que el audio se retrase, y utilizando diferentes loops del audio con el propósito de mostrar la intensidad del nivel, parecido a como lo hace el juego Plantas VS Zombies con su sistema de oleadas, donde cada oleada añade un instrumento más a la banda sonora).

El juego tendrá obstáculos los cuales habrán que destruir para llegar al camino de la llave o el final del nivel, y tendrá un tiempo máximo para cada stage, los enemigos tendrán distintos tipos, con algunos teniendo un patrón de movimiento distinto y algunos con la capacidad de moverse más de una casilla.

Los enemigos deberán hacer pathfinding para ir a atacar al jugador, con un indicador de la dirección en donde estos irán, el jugador tendra movimiento libre, a diferencia de los npcs que deberán moverse al ritmo de la banda sonora.

Mientras se progrese, los enemigos tendrán más vida y los obstáculos serán más dificil de romper, y ahí es donde entrarán las mejoras entre niveles.

    • Las mejoras entre niveles funcionarán así:
        1. Al terminar una stage, el jugador conseguirá una cantidad de dinero dependiendo del tiempo sobrante y la dificultad de la stage.
        2. Con el dinero se podrán comprar mejoras de velocidad, distancia de movimiento y ataque, con las que se podrán recorrer cuadros más rapidamente y acabar con enemigos fácilmente.
        3. Cada 5 rondas se podrá comprar el regresar 1 stage, con el propósito de ahorrar dinero para seguir consiguiendo mejoras.
        
El juego contará con una pantalla de inicio con botones los cuales se podran presionar utilizando el teclado, habrá un boton para el inicio de la partida y para las leaderboards (el cual presentará un top 5 de scores con nombres)

Al inicio de la partida se preguntará la dificultad (lo cual cambiará el tiempo entre los loops de movimiento y la cantidad de vida de los enemigos)

Y por último, al final de la partida se pedirá ingresar un nombre con 4 espacios para letras y/o números para el nombre del jugador, lo cual guardará su score en la leaderboard.
