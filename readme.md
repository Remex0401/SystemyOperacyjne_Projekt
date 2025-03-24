# Problem Jedzących Filozofów

Projekt ten symuluje klasyczny problem jedzących filozofów przy użyciu języka C++ oraz mechanizmów wielowątkowych. Każdy filozof działa jako osobny wątek, przechodząc przez cykl: myślenie → głodny → jedzenie. Synchronizacja odbywa się za pomocą mutexów i semafora, co zapobiega zakleszczeniom i zapewnia, że współdzielone zasoby (np. widelce) są używane w sposób bezpieczny.

---

## Spis Treści

- [Opis projektu](#opis-projektu)
- [Główne Komponenty](#główne-komponenty)
  - [Klasa Semaphore](#klasa-semaphore)
  - [Mutexy](#mutexy)
  - [Funkcja `philosopher`](#funkcja-philosopher)
  - [Funkcja `main`](#funkcja-main)
- [Struktura Kodu](#struktura-kodu)
- [Kompilacja i Uruchamianie](#kompilacja-i-uruchamianie)
---

## Opis projektu

W tym projekcie symulujemy problem jedzących filozofów. Każdy filozof (reprezentowany przez wątek) przechodzi następujące stany:

1. **Myślenie:** Filozof spędza losowy czas na myśleniu.
2. **Głodny:** Filozof staje się głodny i chce jeść. Aby to zrobić, musi uzyskać pozwolenie (od semafora, czyli tzw. "kelnera").
3. **Jedzenie:** Po uzyskaniu pozwolenia filozof próbuje podnieść dwa widelce (mutexy) – lewy i prawy. Jeśli oba są dostępne, przechodzi do fazy jedzenia.
4. **Odkładanie:** Po jedzeniu filozof odkłada widelce i informuje semafor, że zakończył jedzenie, dzięki czemu inny filozof może spróbować jeść.

Celem projektu jest pokazanie, jak synchronizować dostęp do współdzielonych zasobów w programowaniu wielowątkowym, aby uniknąć zakleszczeń i błędów wynikających z jednoczesnego dostępu do zasobów.

---

## Główne Komponenty

### Klasa Semaphore

- **Cel:**  
  Semaphore (semafor) kontroluje, ilu filozofów jednocześnie może próbować jeść. W tym projekcie semafor jest inicjalizowany wartością `n - 1` (dla n filozofów), co zapobiega sytuacji, w której wszyscy filozofowie jednocześnie próbują podnieść widelce i dochodzi do zakleszczenia.

- **Metody:**
  - **`notify()`**  
    Informuje semafor, że filozof skończył jeść i zwalnia zasoby (widelce). Wewnątrz funkcji:
    - Blokowany jest mutex
    - Licznik `count` jest inkrementowany.
    - Wywoływana jest metoda `cv.notify_one()`, która budzi jeden z oczekujących wątków.
  
  - **`wait()`**  
    Wywoływana, gdy filozof jest głodny i chce jeść. Wewnątrz funkcji:
    - Blokowany jest mutex.
    - Wątek czeka (przy użyciu condition variable `cv.wait`) do momentu, aż licznik `count` będzie większy od 0.
    - Po spełnieniu warunku licznik jest dekrementowany, co oznacza, że filozof uzyskał pozwolenie na jedzenie.

### Mutexy

- **Cel:**  
  Mutex (mutual exclusion) zapewnia, że dostęp do współdzielonych zasobów jest chroniony i tylko jeden wątek może korzystać z danego zasobu w danym momencie.
  
- **Przykłady użycia w projekcie:**
  - **Widelce:** Każdy widelec reprezentowany jest przez mutex. Filozof musi „zablokować” (lock) dwa mutexy, aby móc jeść.
  - **`coutMutex`:** Globalny mutex używany przy wypisywaniu komunikatów na konsolę, aby uniknąć mieszania się wypisywanych treści z różnych wątków.

### Funkcja `philosopher`

- **Opis:**  
  Funkcja `philosopher` symuluje cykl życia filozofa:
  1. Filozof myśli przez losowy okres czasu (od 1s do 3s).
  2. Zmienia swój stan na głodny i wywołuje `waiter.wait()`, aby uzyskać pozwolenie na jedzenie.
  3. Próbuje zablokować dwa mutexy reprezentujące lewy i prawy widelec.
  4. Je przez losowy czas (od 1 do 2 sekund).
  5. Po jedzeniu odblokowuje widelce i wywołuje `waiter.notify()`, informując, że zakończył jedzenie.

### Funkcja `main`

- **Opis:**  
  Funkcja `main` jest punktem wejścia do programu i odpowiada za:
  - Inicjalizację liczby filozofów (domyślnie 100, lub podanej jako argument).
  - Utworzenie wektora mutexów (widelce) oraz instancji semafora z wartością `n - 1`.
  - Utworzenie wątków dla każdego filozofa.
  - Synchronizację wątków przez wywołanie `join()`, co powoduje, że główny wątek czeka, aż wszyscy filozofowie zakończą działanie (w tym przypadku działają w nieskończonej pętli, więc program będzie działał nieprzerwanie).

---

## Struktura Kodu

Plik `main.cpp` zawiera następujące sekcje:

1. **Dołączanie bibliotek:**  
   Na początku pliku znajdują się dyrektywy `#include`, które wprowadzają niezbędne biblioteki do obsługi wątków, mutexów, condition variable, wektorów, obsługi czasu i generowania losowych liczb.

2. **Definicja klasy Semaphore:**  
   - Klasa ta zawiera konstruktor oraz metody `wait()` i `notify()`.  
   - Wewnętrzne zmienne (mutex, condition variable, licznik) służą do synchronizacji i kontrolowania dostępu do sekcji krytycznych.

3. **Globalny mutex `coutMutex`:**  
   - Używany do zabezpieczenia wypisywania komunikatów na konsolę, aby komunikaty nie były mieszane między wątkami.

4. **Funkcja `philosopher`:**  
   - Implementuje cykl życia filozofa, czyli symulację myślenia, bycia głodnym, jedzenia i odkładania widelców.
   - W tej funkcji używane są mutexy (dla widelców) i semafor (do kontrolowania liczby filozofów jednocześnie próbujących jeść).

5. **Funkcja `main`:**  
   - Inicjalizuje zmienne, tworzy wektor widelców i semafor.
   - Tworzy wątki dla każdego filozofa i dołącza je przy pomocy `join()`, co zapobiega przedwczesnemu zakończeniu programu.

---

## Kompilacja i Uruchamianie

Aby skompilować projekt, potrzebujesz kompilatora C++ obsługującego standard C++11 lub wyższy oraz wsparcia dla wielowątkowości.

### Przykładowe polecenia kompilacji

#### g++:

g++ -std=c++11 -pthread main.cpp -o dining_philosophers
