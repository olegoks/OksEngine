Механизм предоставляемый стандартом для синхронизации доступа и модификации значений. Может быть реализован с помощью [[Атомарные операции|атомарных инструкций]] и специальных механизмов процессора, которые обеспечивают [[Эксклюзивный доступ к памяти|эксклюзивность]] доступа к [[Ячейка памяти в модели памяти С++|ячейке памяти]], а также [[Барьеры памяти|барьеров памяти]], которые обеспечивают упорядочивание [[Инструкция процессора|инструкций]] и синхронизацию данных между [[Поток|потоками]].
Тип std::atomic предоставляет ряд примитивных операций: `load`, `store`, `fetch_add`, `compare_exchange` и другие. Последние две операции — это read-modify-write операции, атомарность которых обеспечивается специальными [[Инструкция процессора|инструкциями процессора]]. 
**Пример read-modify-write операции, прибавление к числу единицы:**
```С++
static int v1 = 0;
static std::atomic<int> v2{ 0 };

int add_v1() {
    return ++v1;
    /* Generated x86-64 assembly:
        mov     eax, DWORD PTR v1[rip]
        add     eax, 1
        mov     DWORD PTR v1[rip], eax
    */
}

int add_v2() {
    return v2.fetch_add(1);
    /* Generated x86-64 assembly:
        mov     eax, 1
        lock xadd       DWORD PTR _ZL2v2[rip], eax
    */
}
```
В случае с обычной переменной  `v1` типа int имеем три отдельных операций: read-modify-write. Нет гарантий, что другое ядро процессора не выполняет другой операции над `v1`. Операция над `v2` в машинных кодах представлена как одна операция с lock сигналом на уровне процессора, гарантирующим, что к кэш линии, в которой лежит `v2`, эксклюзивно имеет доступ только ядро, выполняющее эту инструкцию.

Случаи, когда синхронизация памяти не требуется:
1. Если все потоки, работающие с одним участком памяти, используют ее только на чтение
2. Если разные потоки используют эксклюзивно разные участки памяти

 Язык C++ предоставляет три способа синхронизации памяти. По мере возрастания строгости: `relaxed`, `release/acquire` и `sequential consistency`.

### Неделимый, но расслабленный
Самый простой для понимания флаг синхронизации памяти — `relaxed`. Он гарантирует только свойство атомарности операций, **при этом не может участвовать в процессе синхронизации данных между потоками**. Свойства:
- модификация переменной "появится" в другом потоке не сразу
- поток `thread2` "увидит" значения **одной и той же** переменной в том же порядке, в котором происходили её  модификации в потоке `thread1`
- порядок модификаций разных переменных в потоке `thread1` не сохранится в потоке `thread2`
Можно использовать `relaxed` модификатор в качестве счетчика. **Пример 1**, [link](https://godbolt.org/z/jx85P9):

```
std::atomic<size_t> counter{ 0 };
 
// process can be called from different threads
void process(Request req) {
	counter.fetch_add(1, std::memory_order_relaxed);
	// ...
}

void print_metrics() {
	std::cout << "Number of requests = " << counter.load(std::memory_order_relaxed) << "\n";
	// ...
}
```

Использование в качестве флага остановки. **Пример 2**, [link](https://godbolt.org/z/o4v3eT):
```
std::atomic<bool> stopped{ false };
 
void thread1() {
	while (!stopped.load(std::memory_order_relaxed)) {
		// ...
	}
}
 
void stop_thread1() {
	stopped.store(true, std::memory_order_relaxed);
}
```
В данном примере не важен порядок в котором `thread1` увидит изменения из потока, вызывающего `stop_thread1`. Также не важно то, чтобы `thread1` мгновенно (синхронно) увидел выставление флага `stopped` в `true`.

Пример **неверного** использования `relaxed` в качестве флага готовности данных. **Пример 3**, [link](https://godbolt.org/z/o3xE4W): ^144ad3

```
std::string data;
std::atomic<bool> ready{ false };
 
void thread1() {
	data = "very important bytes";
	ready.store(true, std::memory_order_relaxed);
}
 
void thread2() {
	while (!ready.load(std::memory_order_relaxed));
	std::cout << "data is ready: " << data << "\n"; // potentially memory corruption is here
}
```
Тут нет гарантий, что поток `thread2` увидит изменения `data` ранее, чем изменение флага `ready`, т.к. синхронизацию памяти флаг `relaxed` не обеспечивает.

## Полный порядок

Флаг синхронизации памяти "единая последовательность" (sequential consistency, `seq_cst`) дает самые строгие. Его свойства:
- порядок модификаций разных атомарных переменных в потоке `thread1` сохранится в потоке `thread2`
- все потоки будут видеть один и тот же порядок модификации всех атомарных переменных. Сами модификации могут происходить в разных потоках
- все модификации памяти (не только модификации над атомиками) в потоке `thread1`, выполняющей `store` на атомарной переменной, будут видны после выполнения `load` этой же переменной в потоке `thread2`

Таким образом можно представить `seq_cst` операции, как барьеры памяти, в которых состояние памяти синхронизируется между всеми потоками программы.

Этот флаг синхронизации памяти в C++ используется по умолчанию, т.к. с ним меньше всего проблем с точки зрения корректности выполнения программы. Но `seq_cst` является дорогой операцией для процессоров, в которых вычислительные ядра слабо связаны между собой в плане механизмов обеспечения консистентности памяти. Например, для x86-64 `seq_cst` дешевле, чем для ARM архитектур.

Продемонстрируем второе свойство. **Пример 4**, из книги, [link](https://godbolt.org/z/T3aT36): ^1449ca

```
std::atomic<bool> x, y;
std::atomic<int> z;
 
void thread_write_x() {
	x.store(true, std::memory_order_seq_cst);
}
 
void thread_write_y() {
	y.store(true, std::memory_order_seq_cst);
}
 
void thread_read_x_then_y() {
	while (!x.load(std::memory_order_seq_cst));
	if (y.load(std::memory_order_seq_cst)) {
		++z;
	}
}
 
 
void thread_read_y_then_x() {
	while (!y.load(std::memory_order_seq_cst));
	if (x.load(std::memory_order_seq_cst)) {
		++z;
	}
}
```

После того, как все четыре потока отработают, значение переменной `z` будет равно `1` или `2`, потому что потоки `thread_read_x_then_y` и `thread_read_y_then_x` "увидят" изменения `x` и `y` в одном и том же порядке. От запуска к запуску это могут быть: сначала `x = true`, потом `y = true`, или сначала `y = true`, потом `x = true`.

Модификатор `seq_cst` всегда может быть использован вместо `relaxed` и `acquire/release`, еще и поэтому он является модификатором по умолчанию. Удобно использовать `seq_cst` для отладки проблем, связанных с [[Гонка за данными(data race)|гонкой данных]] в многопоточной программе: добиваемся корректной работы программы и далее заменяем `seq_cst` на менее строгие флаги синхронизации памяти. **Примеры 1** и **2** также будут корректно работать, если заменить `relaxed` на `seq_cst`, а **пример 3** начнет работать корректно после такой замены.

## Синхронизация пары. Acquire/Release

Флаг синхронизации памяти `acquire/release` является более тонким способом синхронизировать данные между парой потоков. Два ключевых слова: `memory_order_acquire` и `memory_order_release` работают только в паре над одним атомарным объектом. Рассмотрим их свойства:
- модификация атомарной переменной с `release` будет видна видна в другом потоке, выполняющем чтение этой же атомарной переменной с `acquire`
- все модификации памяти в потоке `thread1`, выполняющей запись атомарной переменной с `release`, будут видны после выполнения чтения той же переменной с `acquire` в потоке `thread2`
- процессор и компилятор не могут перенести операции записи в память раньше `release` операции в потоке `thread1`, и нельзя перемещать выше операции чтения из памяти позже `acquire` операции в потоке `thread2`
Важно понимать, что нет полного порядка между операциями над разными атомиками, происходящих в разных потоках. Например, в [[#^1449ca|примере 4]] если все операции `store` заменить на `memory_order_release`, а операции `load` заменить на `memory_order_acquire`, то значение `z` после выполнения программы может быть равно 0, 1 или 2. Это связано с тем, что, независимо от того в каком порядке по времени выполнения выполнены `store` для `x` и `y`, потоки `thread_read_x_then_y` и `thread_read_y_then_x` могут увидеть эти изменения в разных порядках. Кстати, такими же изменениями для `load` и `store` можно исправить [[#^144ad3|пример 3]]. Такое изменение будет корректным и производительными, т.к. тут нам не требуется единый порядок изменений между всеми потоками (как в случае с `seq_cst` ), а требуется синхронизировать память между двумя потоками.

Используя `release`, мы даем инструкцию, что данные в этом потоке готовы для чтения из другого потока. Используя `acquire`, мы даем инструкцию "подгрузить" все данные, которые подготовил для нас первый поток. Но если мы делаем `release` и `acquire` на разных атомарных переменных, то получим [[Неопределенное поведение|UB]] вместо синхронизации памяти.

Рассмотрим реализацию простейшего [[Мьютекс(std_mutex)|мьютекса]], который ожидает в цикле сброса флага для того, чтобы получить `lock`. Такой мьютекс называют `spinlock`. Это не самый эффективный способ реализации мьютекса, но он обладает всеми нужными свойствами, на которые я хочу обратить внимание. **Пример 5**, [link](https://godbolt.org/z/oYbGdK): 

```
class mutex {
public:
	void lock() {
		bool expected = false;
		while(!_locked.compare_exchange_weak(expected, true, std::memory_order_acquire)) {
			expected = false;
		}
	}
 
	void unlock() {
		_locked.store(false, std::memory_order_release);
	}
 
private:
	std::atomic<bool> _locked;
};
```

Функция `lock()` непрерывно пробует сменить значение с false на true с модификатором синхронизации памяти `acquire`. Разница между `compare_exchage_weak` и `strong` незначительна, про нее можно почитать на [cppreference](https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange). Функция `unlock()` выставляет значение в false с синхронизацией `release`. Обратите внимание, что мьютекс не только обеспечивает эксклюзивным доступ к блоку кода, который он защищает. **Он также делает доступным те изменения памяти, которые были сделаны до вызова** `unlock()` **в коде, который будет работать после вызова** `lock()`**.** Это важное свойство. Иногда может сложиться ошибочное мнение, что мьютекс в конкретном месте не нужен.

Рассмотрим такой пример, называемый [[Блокировка с двойной проверкой (Double-Checked Locking)|Double Checked Locking Anti-Pattern]]. **Пример 6**, [link](https://godbolt.org/z/PaYTc6):

```
struct Singleton {
	// ...
};
 
static Singleton* singleton = nullptr;
static std::mutex mtx;
static bool initialized = false;
 
void lazy_init() {
	if (initialized) // early return to avoid touching mutex every call
		return;
 
	std::unique_lock l(mtx); // `mutex` locks here (acquire memory)
	if (!initialized) {
		singleton = new Singleton();
		initialized = true;
	}
	// `mutex` unlocks here (release memory)
}
```

Идея проста: хотим единожды в рантайме инициализировать объект `Singleton`. Это нужно сделать потокобезопасно, поэтому имеем мьютекс и флаг инициализации. Т.к. создается объект единожды, а используется `singleton` указатель в read-only режиме всю оставшуюся жизнь программы, то кажется разумным добавить предварительную проверку `if (initialized) return`. Данный код будет корректно работать на архитектурах процессора с более строгими гарантиями консистентности памяти, например в x86-64. Но данный код неверный с точки зрения стандарта C++. Давайте рассмотрим такой сценарий использования:

```
void thread1() {
	lazy_init();
	singleton->do_job();
}
 
void thread2() {
	lazy_init();
	singleton->do_job();
}
```

Рассмотрим следующую последовательность действий во времени:

1. сначала отрабатывает `thread1` -> выполняет инициализацию под мьютексом:
- lock мьютекса (`acquire`)
- `singleton = ..`
- `initialized = true`
- unlock мьютекса (`release`)
2. далее в игру вступает `thread2`:
- `if(initalized)` возвращает `true` (память, где содержится `initialized` могла быть неявно синхронизирована между ядрами процессора)
- `singleton->do_job()` приводит к `segmentation fault` (указатель `singleton` не обязан был быть синхронизирован с потоком `thread1`)
Этот случай интересен тем, что наглядно показывает роль [[Мьютекс(std_mutex)|мьютекса]] не только как [[Примитив синхронизации||примитива синхронизации]] [[Поток|потока выполнения]], но и синхронизации памяти.