#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <clocale>
#include <cstdlib>

using namespace std;

static int quantityOfDikari, currentquantityOfKuski, currentIndexOfDikar, quantityOfKuski, quantityOfRepeats;
mutex mtx;
thread *threads;

// Генерируем рандомное число между значениями min и max.
int getRandomNumber(int min, int max)
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    // Равномерно распределяем рандомное число в нашем диапазоне
    return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

void EatProcess() {
    mtx.lock(); // Начало критической секции
        currentIndexOfDikar %= quantityOfDikari; // Порядковый номер дикаря
        currentquantityOfKuski -= 1; // Дикарь ест и из горшка вычитается один кусок

        cout << "Дикарь номер " << currentIndexOfDikar + 1 << " съел один кусок. Осталось кусков в горшке : " << currentquantityOfKuski << ". ID потока : " << this_thread::get_id() << endl;
        currentIndexOfDikar += 1;

        if (currentquantityOfKuski == 0) { // В случае, если горшок опустел, вызывает поток для повара
            auto *th = new thread([&]() {
                currentquantityOfKuski = quantityOfKuski; // Повар заполняет горшок обратно
                cout << "Повар только что проснулся и заполнил горшок " << currentquantityOfKuski << " кусками миссионера. ID потока : " << this_thread::get_id() << endl << endl;
            });
            th->join();
            delete th; // Очищаем память от использованного потока повара
        }
    mtx.unlock(); // Конец критической секции
}

// Функция реализована для удобства ограничения количества итераций заполнения/опустошения горшка (пользователь задаёт кол-во итераций в программе)
void repeatEatProcess() {
    for (int loopIterator = 0; loopIterator < quantityOfRepeats; ++loopIterator) {
        for (int i = 0; i < quantityOfDikari; ++i) {
            threads[i] = thread(EatProcess);
        }
        for (int i = 0; i < quantityOfDikari; ++i) {
            threads[i].join();
        }
    }
}

// Грязная работа по взаимодействию с пользователем
void startProgram() {
    int choice;
    cout << "1. Ввести параметры вручную.\n2. Сгенерировать параметры.\n" << "Выберете нужный вариант : ";
    cin >> choice;
    if (choice == 1) {
        cout << "\nВведите количество повторений алгоритма : "; cin >> quantityOfRepeats;
        cout << "Введите количество дикарей : "; cin >> quantityOfDikari;
        cout << "Введите количество кусков в горшке : "; cin >> quantityOfKuski;
        cout << endl;
        currentquantityOfKuski = quantityOfKuski;
        currentIndexOfDikar = 0;
        threads = new thread[quantityOfDikari];
    } else if (choice == 2) {
        quantityOfRepeats = getRandomNumber(1, 50);
        quantityOfDikari = getRandomNumber(1, 50);
        quantityOfKuski = getRandomNumber(1, 50);
        currentquantityOfKuski = quantityOfKuski;
        currentIndexOfDikar = 0;
        threads = new thread[quantityOfDikari];
        cout << "\nСгенерированные данные следующие : \nКоличество повторений алгоритма : "
                 << quantityOfRepeats << "\nКоличество дикарей : " << quantityOfDikari << "\nКоличество кусков в горшке : "
                 << quantityOfKuski << endl << endl;
        this_thread::sleep_for(chrono::milliseconds(2000));
    } else {
        cout << "Неправильный ввод! Перезапустите программу и попробуйте снова!";
        exit(0);
    }
}

void getTime(std::chrono::nanoseconds elapsed_ms) {
    if (elapsed_ms.count() > 1000000) {
        cout << endl << "Конечное время работы программы : " << elapsed_ms.count() / 1000000 << " ms\n";
    } else {
        cout << endl << "Конечное время работы программы : " << "0." << elapsed_ms.count() << " ms\n";
    }
}

int main() {

    setlocale(LC_ALL, "Russian"); // Локализация
    startProgram();

    auto begin = std::chrono::steady_clock::now(); // Старт отсчета времени

    repeatEatProcess();
    delete []threads;

    auto elapsed_ms = chrono::duration_cast<std::chrono::nanoseconds>(chrono::steady_clock::now() - begin);
    getTime(elapsed_ms);

    return 0;
}