#include <iostream>
#include <thread>
#include <map>
#include <string>
#include <sstream>
#include <windows.h>
#include <fstream>
#include <functional>


using namespace std;

bool running = true, paused = false;
int currentLogger = 0, eventCount = 0;
int interval = rand() % 500 + 100;
HANDLE readPipe, writePipe;


struct Event {
	time_t currentTime;
	int identifEvents;
	int firstParam;
	int secondParam;
	int thirdParam;
};


class Logger
{
protected:
	ofstream myFile;
	Logger(const char* fileName) {
		myFile.open(fileName, ios::app);
		if (!myFile.is_open()) {
			cerr << "не удалось открыть файл" <<  endl;
		}
	}
	virtual ~Logger() {
		if (myFile.is_open()) {
			myFile.flush();  // Явная запись буфера
			myFile.close();
		}
	}
public:
	static Logger* GetLogger(int level);
	virtual void Write(const Event& event)=0;
};

class Logger0 : public Logger {
public:
	Logger0() : Logger("log0.txt") { }
	void Write(const Event& event) override {
		time_t now = time(0);
		tm ltm;
		localtime_s(&ltm, &now);
		myFile << ltm.tm_hour << "." << ltm.tm_min << " - " << event.identifEvents << endl;
	}
};

class Logger1 : public Logger {
public:
	Logger1() : Logger("log1.txt") { }
	void Write(const Event& event) override { 
		time_t now = time(0);
		tm ltm;
		localtime_s(&ltm, &now);		
		myFile << ltm.tm_hour << "." << ltm.tm_min << " - " << event.identifEvents << " - " << event.firstParam << endl;
	}
};

class Logger2 : public Logger {
public:
	Logger2() : Logger("log2.txt") { }
	void Write(const Event& event) override { 
		time_t now = time(0);
		tm ltm;
		localtime_s(&ltm, &now);
		myFile << ltm.tm_hour << "." << ltm.tm_min << " - " << event.identifEvents << " - " << event.firstParam << " - " << event.secondParam << " - " << event.thirdParam << endl;
	}
};

Logger* Logger::GetLogger(int level) {
	switch (level) {
	case 0: return new Logger0();
	case 1: return new Logger1();
	case 2: return new Logger2();
	default: return new Logger0();
	}
}

void eventGenerator() {
	while (running) {
		if (!paused) {
			Event event;
			event.currentTime = time(0);
			event.identifEvents = eventCount++;
			event.firstParam= rand();
			event.secondParam = rand();
			event.thirdParam = rand();

			WriteFile(writePipe, &event, sizeof(event), nullptr, NULL);
		}
		this_thread::sleep_for(chrono::milliseconds(interval));
	}
}

void eventLogger() {
	Logger* logger = Logger::GetLogger(currentLogger);
	while (running) {
		Event event;
		if (ReadFile(readPipe, &event, sizeof(event), nullptr, NULL)) {
			logger = Logger::GetLogger(currentLogger);
			logger->Write(event);
		}
		else {
			this_thread::sleep_for(chrono::milliseconds(interval));
		}
	}
}

void routine(map<std::string, function<void(const char*)>>& commands) {
	commands["date"] = [](const char*) { 
		time_t now = time(0); 
		tm ltm;
		localtime_s(&ltm, &now);
		cout << "текущая дата: " << ltm.tm_mday << "." << 1 + ltm.tm_mon << "." << 1900 + ltm.tm_year << endl; 
		};
	commands["time"] = [](const char*) { 
		time_t now = time(0); 
		tm ltm;
		localtime_s(&ltm, &now);
		cout << "текущее время: " << ltm.tm_hour << "." << ltm.tm_min << endl; 
		};
	commands["exit"] = [](const char*) { 
		running = false; 
		};
	commands["faster"] = [](const char*) { 
		if (interval > 100) interval -= 50;
		};
	commands["slower"] = [](const char*) { 
		interval += 50;
		};
	commands["pause"] = [](const char*) { 
		paused = true; 
		};
	commands["resume"] = [](const char*) { 
		paused = false; 
		};
	commands["level0"] = [](const char*) { 
		currentLogger = 0; 
		};
	commands["level1"] = [](const char*) { 
		currentLogger = 1; 
		};
	commands["level2"] = [](const char*) {
		currentLogger = 2; 
		};
	commands["stat"] = [](const char*) { 
		cout << "кол-во событий: " << eventCount << endl; 
		};
}

 int main()
{
	 setlocale(LC_ALL, "");
	srand(time(0));
	thread generator(eventGenerator);
	thread logger(eventLogger);

	map<string, function<void(const char*)>> commands; //functional для создания шаблона (команды)
	routine(commands);

	string command, console, strAfterSpace;
	const char* parameter;
	while (running) {
		getline(cin, console);
		istringstream potok(console);
		potok >> command;

		getline(potok, strAfterSpace);
		strAfterSpace.erase(0, strAfterSpace.find_first_not_of(" "));
		parameter = strAfterSpace.empty() ? nullptr : strAfterSpace.c_str();


		if (commands.count(command)) {
			commands[command](parameter);
		}
		else {
			cout << "неизвестная команда" << endl;
		}
	}
	generator.join();
	logger.join();
	CloseHandle(readPipe);
	CloseHandle(writePipe);
	return 0;
}