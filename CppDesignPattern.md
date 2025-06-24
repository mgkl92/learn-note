# C++ 设计模式

## 面向对象设计原则

### 依赖倒置原则（DIP）

高层模块不应该依赖于底层模块，两个都应该依赖于抽象。

抽象不依赖于细节，而细节应该依赖于抽象。

### 开放封闭原则（OCP）

- 对拓展开放

- 对修改关闭

### 单一职责原则

### Liskov 替换原则

### 接口隔离原则

客户端不应该依赖于它不需要的接口。

### 合成复用原则



## 设计模式分类

### 组件协作 3

- 模板方法模式

    在模板模式中，一个抽象类公开定义了执行它的方法的方式或模板，其子类可以按需求重写方法实现。

    模板模式主要用来解决多个子类中重复实现相同方法的问题，通过将通用方法抽象到父类中来避免代码的重复。
    
    ```cpp
    // v1
    // 完成某件事需要多个步骤
    class Library {
    public:
        void Step1() { ... }

        void Step3() { ... }

        void Step5() { ... }
    };

    // 但每个步骤根据对象的不同，实现不同
    class Application : public Library {
    protected:
        virtual bool Step2() {
            ...
            return true;
        }

        virtual bool Step4() {
            ....
        }
    };

    // v2
    class Library {
    public:
        // 2. 并提供完成该事件的总接口
        void run() {
            Step1();
            
            if (Step2()) {
                Step3();
            }

            for (int i = 0; i < 4; ++i) {
                Step4();
            }

            Step5();
        }

    protected:
        // 1. 在父类中将稳定方法和变化方法分离
        // 稳定方法
        void Step1() { ... }

        void Step3() { ... }

        void Step5() { ... }
        
        // 变化方法
        virtual bool Step2() = 0;
        virtual void Step4() = 0;
    };

    // 3. 不同子类根据其需求实现变化方法
    class Application : public Library {
    protected:
        virtual bool Step2() override {
            // 重新实现
            ...
            return true;
        }

        virtual void Step4() override {
            // 重新实现
            ...
        }
    };
    ```


- 观察者模式

    观察者模式定义了一种一对多的依赖关系，当一个对象的状态发变化时，其所有的依赖着都会收到通知并自动更新。

    ```cpp
    class FileSplitter {
        string filePath;
        int fileNumber;
        ProgressBar* progressBar;

    public:
        FileSplitter(const string& filePath, int fileNumber, ProgressBar* progressBar) {
            this->filePaht = filePath;
            this->fileNumber = fileNumber;
            this->progressBar = progressBar;
        }

        void split() {
            // 1. 读取文件

            // 2. 分批次写入
            for (int i = 0; i < fileNumber; ++i) {
                float progressValue = fileNumber;
                progressValue = (i + 1) / progressValue;
                progressBar->setValue(progressValue);
            }
        }
    };

    class IProgress {
    public:
        virtual void DoProgress(float value) = 0;
        virtual ~IProgress() {}
    };

    class FileSplitter {
        string filePath;
        int fileNumber;
        
        // 支持多个观察者
        List<IProgress*> progressLists;

    protected:
        virtual void onProgress(float value) {
            List<IProgress*>::iterator it = progressLists.begin();

            while (it != progressLists.end()) {
                (*it)->DoProgress(value); // 更新进度条
                ++it;
            }
        }

    public:
        FileSplitter(const string& filePath, int fileNumber) {
            this->filePath = filePath;
            this->fileNumber = fileNumber;
        }

        void split() { ... }

        void addProgress(IProgress* progress) { 
            progressLists.push_back(progress); 
        }

        void removeProgress(IProgress* progress) { 
            progressLists.remove(progress); 
        }
    };

    // v1
    class MainForm : public Form {
        TextBox* txtFilePath;
        TextBox* txtFileNumber;
        ProgressBar* progressBar;

    public:
        void click() {
            string filePath = txtFilePath->getText();
            int number = atoi(txtFileNumber->getText().c_str());

            FileSplitter splitter(filePath, number, progressBar);

            splitter.split();
        }
    };

    // v2
    class MainForm : public Form,  public IProgress {
        TextBox* txtFilePath;
        TextBox* txtFileNumber;
        ProgressBar* progressBar;

    public:
        void click() {
            string filePaht = txtFilePath->getText();
            int number = atoi(txtFileNumber->getText().c_str());

            ConsoleNotifier cn;

            FileSplitter splitter(filePath, number);

            splitter.addProgress(this);
            splitter.addProgress(&cn);

            splitter.split();
            splitter.removeProgress(this);
        }

        virtual void DoProgress(float value) {
            progressBar->setValue(value);
        }
    };

    class ConsoleNotifier : public IProgress {
    public:
        virtual void DoProgress(float value) { ... }
    };
    ```

- 策略模式

    在策略模式中一个类的行为或其算法可以在运行时更改。

    ```cpp
    enum TaxBase {
        CN_TAX,
        US_TAX,
        DE_TAX,
        FR_TAX
    };

    class SalesOrder {
        TaxBase taxBase;

    public:
        double calculateTax() {
            if (taxBase == CN_TAX) { ... }
            else if (taxBase == US_TAX) { ... }
            else if (taxBase == DE_TAX) { ... }
            else if (taxBase == FR_TAX) { ... }
        }
    };

    class TaxStrategy {
    public:
        virtual double calculate(const Context& context) = 0;
        virtual ~TaxStrategy() {}
    };

    class CNTAX : public TaxStrategy {
    public:
        virtual double calculate(const Context& context) { ... }
    };

    class USTAX : public TaxStrategy {
    public:
        virtual double calculate(const Context& context) { ... }
    };

    class DETAX : public TaxStrategy {
    public:
        virtual double calculate(const Context& context) { ... }
    };

    class FRTAX : public TaxStrategy {
    public:
        virtual double calculate(const Context& context) { ... }
    };

    class SalesOrder {
        TaxStrategy* strategy;

    public:
        SalesOrder(StrategyFactory* strategyFactory) {
            this->strategy = strategyFactory->NewStrategy();
        }

        ~SalesOrder() { delete strategy; }

    public:
        double calculateTax() {
            ...
            Context contex;

            double val = strategy->calculate(context);
        }
    };
    ```

### 单一职责 2

- 装饰器模式

    装饰器模式允许在不改变一个对象结构的情况下，为其添加新的功能。

    ```cpp
    // v1：基于实现类进行拓展
    // 抽象接口
    class Stream {
    public:
        virtual char read(int number) = 0;
        virtual void seek(int position) = 0;
        virtual void write(char data) = 0;

        virtual ~Stream();
    };

    // 实现类
    class FileStream : public Stream {
    public:
        virtual char read(int number) override { ... }
        virtual void seek(int position) override { ... }
        virtual void write(char data) override { ... }
    };

    class NetworkStream : public Stream {
    public:
        virtual char read(int number) override { ... }
        virtual void seek(int position) override { ... }
        virtual void write(char data) override { ... }
    };

    // 拓展 1
    class CryptoFileStream : public FileSteam {
    public:
        virtual char read(int number) {
            // 加密操作
            FileStream::read(number);
        }

        virtual void seek(int position) {
            // 加密操作
            FileStream::seek(position);
        }

        virtual void write(char data) {
            // 加密操作
            FileStream::write(data);
        }
    };

    class CryptoNetworkStream : public NetworkStream {
    public:
        virtual char read(int number) {
            // 加密操作
            NetworkStream::read(number);
        }

        virtual void seek(int position) {
            // 加密操作
            NetworkStream::seek(position);
        }

        virtual void write(char data) {
            // 加密操作
            NetworkStream::write(data);
        }
    };

    // 拓展 2
    class BufferedFileStream : public FileStream { ... };
    class BufferedNetworkStream : public NetworkStream { ... };

    // 拓展 3
    class CryptoBufferedFileStream : public FileStream { ... };
    class CryptoBUfferedNetworkStream : public NetworkStream { ... };

    // v2：基于抽象类进行拓展
    class CryptoStream : public Stream {
        Stream* stream;

    public:
        CryptoStream(Stream *stream) { this->stream = stream; }

        virtual char read(int number) override {
            // 加密操作
            stream->read(number);
        }

        virtual void seek(int position) override {
            // 加密操作
            stream->seek(position);
        }

        virtual void write(char data) override {
            // 加密操作
            stream->write(data);
        }
    };

    class BufferedStream : public Stream {
        Stream* stream;

    public:
        BufferedStream(Stream* stream) { this->stream = stream; }
        
        virtual char read(int number) override {
            // Buffer 操作
            stream->read(number);
        }

        virtual void seek(int position) override {
            // Buffer 操作
            stream->seek(position);
        }

        virtual void write(char data) override {
            // Buffer 操作
            stream->write(data);
        }
    };

    // v3: 进一步抽象化
    class DecoratorStream : public Stream {
    protected:
        Stream* stream;
        DecoratorStream(Stream* stream) { this->stream = stream; }
    };

    class CryptoStream : public DecoratorStream {
    public:
        CryptoStream(Stream* stream) : DecoratorStream(stream) {}

        virtual char read(int number) override {
            // 加密操作
            stream->read(number);
        }

        virtual void seek(int position) override {
            // 加密操作
            stream->seek(position);
        }

        virtual void write(char data) override {
            // 加密操作
            stream->write(data);
        }
    };

    class BufferedStream : public DecoratorStream {
    public:
        BufferedStream(Stream* stream) : DecoratorStream(stream) {}

        virtual char read(int number) override {
            // 加密操作
            stream->read(number);
        }

        virtual void seek(int position) override {
            // 加密操作
            stream->seek(position);
        }

        virtual void write(char data) override {
            // 加密操作
            stream->write(data);
        }
    };
    ```

- 桥接模式

    桥接模式用于将抽象化与实现化解耦，使得二者可以独立变化。

    ```cpp
    // v1
    // 抽象类
    class Messager {
    public:
        virtual void login(string username, string password) = 0;
        virtual void sendMessage(string message) = 0;
        virtual void sendPicture(Image image) = 0;

        virtual void playSound() = 0;
        virtual void drawShape() = 0;
        virtual void writeText() = 0;

        virtual ~Messager() {}
    }; 

    // 平台相关实现
    class PCMessagerBase : public Messager {
    public:
        virtual void playSound() override { ... }
        virtual void drawShape() override { ... }
        virtual void writeText() override { ... }
    };

    class MobileMessagerBase : public Messager {
    public:
        virtual void playSound() override { ... }
        virtual void drawShape() override { ... }
        virtual void writeText() override { ... }
    };

    // 业务抽象
    // 轻量化实现
    class PCMessagerLite : public PCMessagerBase {
    public:
        virtual void login(string username, string password) { ... }
        virtual void sendMessage(string message) { ... }
        virtual void sendPicture(Image image) { ... }
    };

    // 完整实现，附加更多功能
    class PCMessagerPerfect public : PCMessagerBase {
        virtual void login(string username, string password) { ... }
        virtual void sendMessage(string message) { ... }
        virtual void sendPicture(Image image) { ... }
    };

    class MobileMessagerLite : public MobileMessagerBase {
    public:
        ...
    };

    class MobileMessagerPerfect : public MobileMessagerBase {
    public:
        ...
    };

    // v2
    // 将不同的变化方向（平台和业务）分离
    class MessagerImp;
    class Messager {
    protected:
        MessagerImp* messagerImp;

    public:
        virtual void login(string username, string password) = 0;
        virtual void sendMessage(string message) = 0;
        virtual void sendPicture(Image image) = 0;

        virtual ~Messager() {}
    };

    class MessagerImp {
    public:
        virtual void playSound() = 0;
        virtual void drawShape() = 0;
        virtual void writeText() = 0;

        virtual ~MessagerImp() {}
    };

    // 平台实现
    class PCMessagerImp : public MessagerImp {
    public:
        virtual void playSound() override { ... }
        virtual void drawShape() override { ... }
        virtual void writeText() override { ... }
    };

    class MobileMessagerImp : public MessagerImp {
    public:
        virtual void playSound() override { ... }
        virtual void drawShape() override { ... }
        virtual void writeText() override { ... }
    };

    // 业务实现
    class MessagerLite : public Messager {
    public:
        virtual void login(string username, string password) { ... }
        virtual void sendMessager(string message) { ... }
        virtual void sendPicture(Image image) { ... }
    };

    class MessagerPerfect : public Messager {
        virtual void login(string username, string password) { ... }
        virtual void sendMessager(string message) { ... }
        virtual void sendPicture(Image image) { ... }
    };

    void process() {
        // 运行时装配
        MessagerImp* mImp = new PCMessagerImp();
        Messager* m = new Messager(mImp);
    }
    ```

### 对象创建 5

- 工厂方法

    工厂模式提供了一种创建对象的方式，使得对象的创建过程与使用过程分离。

    简单工厂模式使用一个单独的工厂来创建不同的对象，根据传入的参数决定创建那种类型的对象。

    工厂方法模式定义了一个创建对象的接口，由子类决定实例化那个类，该模式将对象的创建延迟到子类。

    工厂方法模式的缺点在于要求创建对象的方法参数相同。

    ```cpp
    // 抽象接口
    class ISplitter {
    public:
        virtual void split() = 0;
        virtual void ~ISplitter() {}
    };

    // 抽象接口的实现类
    class BinarySplitter : public ISplitter { ... };

    class TxtSplitter : public ISplitter { ... };

    // 工厂类的抽象接口
    class SplitterFactory {
    public:
        virtual ISplitter* createSplitter() = 0;
        virtual ~SplitterFactory() {}
    }

    // 工厂类的实现类
    class BinarySplitterFactory : public SplitterFactory {
    public:
        virtual ISplitter* createSplitter() {
            return new BinarySplitter();
        }
    };

    class TxtSplitterFactory : public SplitterFactory {
    public:
        virtual ISplitter* createrSplitter() {
            return new TxtSplitter();
        } 
    };
    ```

- 抽象工厂模式

    抽象工厂模式提供一个创建一些列相关或互相依赖对象的接口，而无需指定它们具体的类。

    ```cpp
    // v1
    class EmployeeDAO {
    public:
        vector<EmployeeDO> getEmployees() {
            SqlConnection* connection = new SqlConnection();
            connection->connectionString = "...";

            SqlCommand* command = new SqlCommand();
            command->commandText = "...";
            command->setConnection(connection);

            SqlDataReader* reader = command->executeReader();
            while (reader->Read()) {
                ...
            }
        }
    };
    ```

    ```cpp
    // v2 分别使用单独的工厂方法
    class IDBConnection { ... };
    class IDBConnectionFactory {
    public:
        virtual IDBConnection* createDBConnection() = 0;
    };

    class IDBCommand { ... };
    class IDBCommandFactory {
    public:
        virtual IDBCommand * createDBCommand() = 0;
    };

    class IDataReader { ... };
    class IDataReaderFactory {
    public:
        virtual IDataReader* createDataReader() = 0;
    };

    // SQL Server 
    class SqlConnection : public IDBConnection { ... };
    class SqlConnectionFactory : public IDBConnectionFactory { ... };

    class SqlCommand : public IDBCommand { ... };
    class SqlCommandFactory : public IDBCommandFactory { ... };

    class SqlDataReader : public IDataReader { ... };
    class SqlDataReaderFactory : public IDataReaderFactory { ... };

    // Oracle
    class OracleConnection : public IDBConnection { ... };
    class OracleConnectionFactory : public IDBConnectionFactory { ... };

    class OracleCommand : public IDBCommand { ... };
    class OracleCommandFactory : public IDBCommandFactory { ... };

    class OracleDataReader : public IDataReader { ... };
    class OracleDataReaderFactory : public IDataReaderFactory { ... };

    class EmployeeDAO {
        IDBConnectionFactory *dbConnectionFactory;
        IDBCommandFactory* dbCommandFactory;
        IDataReaderFactory* dataReaderFactory;
        
    public:
        vector<EmployeeDO> getEmployees() {
            IDBConnection* connection = dbConnectionFactory->createDBConnection();
            connection->connectionString("...");

            IDBCommand* command = dbCommandFactory->createDBCommand();
            command->commandText("...");
            command->setConnection(connection); // 关联性

            IDataReader* reader = command->executeReader();
            while (reader->Read()) {
                ...
            }
        }
    };
    ```

    ```cpp
    class IDBFactory {
    public:
        virtual IDBConnection* createDBConnection() = 0;
        virtual IDBCommand* createDBCommand() = 0;
        virtual IDataReader* createDataReader() = 0;
    };

    // SQL Server
    class SqlDBFactory : public IDBFactory {
    public:
        virtual IDBConnection* createDBConnection() { ... };
        virtual IDBCommand* createDBCommand() { ... };
        virtual IDataReader* createDataReader() { ... };
    };

    // Oracle
    class OracleDBFactory : public IDBFactory {
        irtual IDBConnection* createDBConnection() { ... };
        virtual IDBCommand* createDBCommand() { ... };
        virtual IDataReader* createDataReader() { ... };
    };

    class EmployeeDAO {
        IDBFactory* dbFactory;

    public:
        vector<EmployeeDO> getEmployees() {
            IDBConnection* connection = dbFacotry->createDBConnection();
            connection->connectionString("...");

            IDBCommand* command = dbFactory->createDBCommand();
            command->commandText("...");
            command->setConnection(connection); // 关联性

            IDataReader* reader = command->executeReader(); // 关联性
            while (reader->Read()) {
                ...
            }
        }
    };
    ```

- 原型模式

    实现一个原型接口，该接口用于创建当前对象的克隆。

    ```cpp
    class ISplitter {
    public:
        virtual void split() = 0;

        virtual ISplitter* clone() = 0; // 通过克隆自身来创建对象

        virtual ~ISpliter() {}
    };

    class BinarySplitter : public ISplitter {
    public:
        virtual ISplitter* clone() { // 接口实现
            return new BinarySplitter(*this); 
        }
    };

    class TxtSpliiter : public ISplitter {
    public:
        virtual ISplitter* clone() {
            return new TxtSpliter(*this);
        }
    };
    ```

- 建造者模式

    将一个复杂对象的构建过程与其表示相分离，从而可以创建具有不同表示形式的对象。

    ```cpp
    class House {
        ...
    };

    class HouseBuilder {
    public:
        House* getResult() {
            return pHouse;
        }

        virtual ~HouseBuilder() { ... }

    protected:
        House* pHouse;

        virtual void buildPart1() = 0;
        virtual void buildPart2() = 0;
        virtual void buildPart3() = 0;
        virtual void buildPart4() = 0;
        virtual void buildPart5() = 0;
    };

    class StoneHouse : public House { ... };

    class StoneHouseBuilder : public HouseBuilder {
    protected:
        virtual void buildPart1() { ... };
        virtual void buildPart2() { ... };
        virtual void buildPart3() { ... };
        virtual void buildPart4() { ... };
        virtual void buildPart5() { ... };

    };

    class HouseDirector {
    public:
        HouseBuilder* pHouseBuilder;
        
        HouseDirector(HouseBuilder* pHouseBuilder) {
            this->pHouseBuilder = pHouseBuilder;
        }

        House* Conturct() {
            pHouseBuilder->buildPart1();

            for (int i = 0; i < 4; ++i) {
                pHouseBuilder->buildPart2();
            }

            bool flag = pHouseBuilder->buildPart3();
            if (flag) {
                pHouseBuilder->buildPart4();
            }

            pHouseBuilder->buildPart5();

            return pHouseBuilder->getResult();
        }
    };
        ```

### 对象性能 2

- 单例模式

    一个类只有一个实例，并提供一个全局的访问点来访问该实例。

    ```cpp
    class Singleton {
    private:
        Singleton();
        Singleton(const Singleton& other);
        
        static Singleton* instance;

    public:
        static Singleton* getInstance();
    };

    Singleton* Singleton::instance = nullptr;

    // 非线程安全
    Singleton* Singleton::getInstance() {
        if (!instace) {
            instance = new Singleton();
        }

        return instance;
    }

    // 线程安全
    Singleton* Singleton::getInstance() {
        Lock lock;

        if (!instance) {
            instance = new Singleton();
        }

        return instance;
    }

    // 双检查锁，但存在编译器对读写重排序，不安全
    Singleton* Singleton::getInstance() {
        if (!instance) {
            Lock lock;
            if (!instance) {
                instance = new Singleton();
            }
        }
    }

    // C++ 11 (跨平台实现)
    std::atomic<Singleton*> Singleton::instance;
    std::mutex Singleton::mut;
    
    Singleton* Singleton::getInstance() {
        Singleton *tmp = instance.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        if (!temp) {
            std::lock_guard<std::mutex> lock(mut);
            tmp = instance.load(std::memory_order_relaxed);

            if (!temp) {
                temp = new Singleton();
                std::atomic_thread_fence(std::memory_order_release);
                instance.store(tmp, std::memory_order_relaxed);
            }
        }

        return temp;
    }

    class Singleton {
    private:
        Singleton() {}

        Singleton(const Singleton &) = delete;
        
        Singleton& operator=(const Singleton &) = delete;
    
    public:
        static Singleton& getInstance() {
            // 利用 C++ 局部静态变量的线程安全性
            static Singleton instance;
            return instance;
        }
    };
    ```

- 享元模式

    用于减少创建对象的数量，以减少内占用和提高性能。享元模式尝试重用现有的同类对象，如果未找到匹配的对象，则创建新的对象。
        
    ```cpp
    class Font {
    private:
        string key;

    public:
        Font (const string& key) { ... };
    };

    class FontFactory {
    private:
        std::map<string, Font*> fonts;

    public:
        Font* getFont(const string& key) {
            auto item = fonts.find(key);

            if (item == fonts.end()) {
                Font* font = new Font(key);
                fonts[key] = font;

                return font;
            }

            return fonts[key];
        }

        ...
    };
    ```

### 接口隔离 4

- 代理模式

    代理模式通过引入一个代理对象来控制对原对象的访问，代理对象在客户端和目标对象之间充当中介，负责将客户端的请求转发给目标对象，同时可以在转发请求前后进行额外的处理。

    ```cpp
    class ISubject {
    public:
        virtual void process();
    };

    class RealSubject : public ISubject {
    public:
        virtual void process() { ... }
    };

    class ClientApp {
        ISubject* subject;

    public:
        ClientApp() {
            subject = new RealSubject();
        }

        void doTask() {
            subject->process();
        }
    };

    // 代理类 
    class SubjectProxy : public ISubject {
    public:
        virtual void process() {
            // 将处理请求转发给 RealSubject 对象
            // 将处理结果返回给客户端对象
        }
    };

    class ClientApp{
        ISubject* subject;

    public:
        ClientApp() {
            subject = new SubjectProxy();
        }

        void doTask{
            subject->process();
        }
    };
    ```

- 适配器模式

    适配器模式充当两个不兼容接口之间的桥梁，它果果一个中间件（适配器）将一个类的接口转换成客户端期望的另外一个接口，使得原本不能一起工作的类能够协同工作。

    ```cpp
    // 遗留接口
    class IAdaptee {
    public:
        virtual void foo(int data) = 0;
        virtual int bar() = 0;
    };

    // 遗留类
    class OldClass : public IAdaptee { ... };

    // 目标接口
    class ITarget {
    public:
        virtual void process() = 0;
    };

    // 适配器类
    class Adapter : public ITarget {
    protected:
        IAdaptee* pAdaptee; // 组合

    public:
        Adapter(IAdaptee* pAdaptee) {
            this->pAdaptee = pAdaptee;
        }

        virtual void process() {
            int data = pAdaptee->bar();
            pAdaptee->foo(data);
        }
    };
    ```

- 门面模式

- 中介者模式

    中介者模式用来降低多个对象和类之间的通信复杂性。

    中介者模式定义了一个中介对象来封装一系列对象之间的交互，中介者使得各对象之间不需要显式的相互引用，从而使其能够松散耦合。

### 状态变化

- 备忘录模式

    在不破坏封装性的前提下，备忘录模式用于捕获一个对象的某个状态，并在需要的时候将该对象恢复到原先保存的状态。

    ```cpp
    class Memento {
        string state;

    public:
        Memento(const string & state) { this->state = state; }

        string getState() const { return state; }

        void setState(const string& state) { this->state = state; }
    };


    class Originator {
        string state;
        ...

    public:
        Originator() {}
        createMemnto() {
            return Memento(state);
        }

        void setMemento(const Memento& memento) {
            state = memento.state;
        }
    };

    int main(int argc, const char * argv[]) {
        Originator originator;

        Memento mem = originator.createMemento();

        // 改变 originator 状态
        ...

        // 从备忘录 mem 中恢复原先状态
        originator.setMemento(mem);
    }
    ```

- 状态模式

    状态模式允许对象在内部状态变化时改变其行为，使得对象能够在不同的状态下表现出不同的行为。

    状态模式将每个状态封装成独立的类，从而避免使用大量的条件语句来实现状态的切换！


    ```cpp
    enum NetworkState {
        OPEN,
        CLOSE,
        CONNECT
    };

    class NetworkProcessor {
        NetworkState state;

    public:
        void operation1() {
            if (state == OPEN) {
                ...
                state == CLOSE;
            } else if (state == CLOSE) {
                ...
                state = CONNECT;
            } else if (state == CONNECT) {
                ...
                state = OPEN;
            }
        }

        void operation2() {
            if (state == OPEN) {
                ...
                state = CONNECT;
            } else if (state == CLOSE) {
                ...
                state = OPEN;
            } else if (state == CONNECT) {
                ...
                state = CLOSE;
            }
        }
    };

    // 状态模式
    class NetworkState {
    public:
        NetworkState *pNext;

        virtual void operation1() = 0;
        virtual void operation2() = 0;
        virtual ~NetworkState() {}
    };

    class OpenState : public NetworkState {
        static NetworkState* instance;

    public:
        static NetworkState* getInstance() {
            if (!instance) {
                instance = new OpenState();
            }

            return instace;
        }

        void operation1() {
            ...
            pNext = CloseState::getInstance();
        }

        void operation2() {
            ...
            pNext = ConnectState::getInstance();
        }

        void operation3() {
            ...
            pNext = OpenState::getInstance();
        }
    };

    class CloseState : public NetWorkState { ... };

    class NetworkProcessor {
        NetworkState* pState;

    public:
        NetworkProcessor(NetworkState* pState) {
            this->pState = pState;
        }

        void operation1() {
            ...
            pState->operation1();
            pState = pState->pNext;
            ...
        }

        void operation2() {
            ...
            pState->operation2();
            pState = pState->pNext;
            ...
        }

        void operation3() {
            ...
            pState->operation3();
            pState = pState->pNext;
            ...
        }

    };
    ```

### 数据结构

- 组合模式

    组合模式，又称部分整体模式，用于将一组相似的对象当作单一的对象处理。

    组合模式依据树形结构来组合对象，用来表示部分及整体层次。

    ```cpp
    class Component {
    public:
        virtual void process() = 0;
        virtual ~Component() {}
    };

    class Composite : public Component {
        string name;
        list<Component*> elements;

    public:
        Composite(const string & name) {
            this->name = name;
        }

        void add(Component* element) {
            elements.push_back(element);
        }

        void remove(Component* element) {
            elements.remove(element);
        }

        void process() {
            // 处理当前结点
            ...

            // 处理叶子结点
            for (auto &element : elements) {
                element->process();
            }
        }
    };

    class Leaf : public Component {
        string name;

    public:
        Leaf(const string& name) {
            this->name = name;
        }

        void process() { ... }
    };

    void Invoke(Component & component) {
        ...
        component.process();
        ...
    }

    int main(int argc, const char * argv[]) {

        Composite root("root");

        Composite node1("node1");
        Composite node2("node2");
        Composite node3("node3");
        Composite node4("node4");

        Leaf leaf1("leaf1");
        Leaf leaf2("leaf2");

        root.add(&node1);
        node1.add(&node2);
        node2.add(&leaf1);

        root.add(&node3);
        node3.add(&node4);
        node4.add(&leaf2);

        invoke(root);
        invoke(leaf2);
        invoke(node3);

        return 0;
    }
    ```

- 迭代器模式

    迭代器模式提供一种顺序访问集合对象中的各个元素，而不暴露集合内部的表示形式。

    ```cpp
    template<typename T>
    class Iterator {
        virtual void first() = 0;
        virtual void next() = 0;
        virtual bool isDone() const = 0;
        virtual T& current() = 0;
    };

    template<typename T>
    class MyCollection {
    public:
        Iterator<T> getIterator() { ... }
    };

    template<typename T>
    class CollectionIterator : public Iterator<T> {
        MyCollection<T> container;

    public:
        CollectionIterator(const MyCollection<T> & container) {
            this->container = container;
        }

        void first() override { ... }

        void next() override { ... }

        bool isDone() const override { ... }

        T& current() override { ... }
    };
    ```

- 责任链模式

    责任链模式通过将多个处理器（处理对象）以链式结构连接起来，使得请求沿着这条链传递，直到有一个处理器处理该请求为止。

    责任链模式避免了请求发送者与请求接收者之间的紧耦合。

    ```cpp
    enum class RequestType {
        REQ_HANDLER_1,
        REQ_HANDLER_2,
        REQ_HANDLER_3
    };

    class Request  {
        string description;
        RequestType reqType;
    public:
        Request(const string& description, RequestType reqType) {
            this->description = description;
            this->reqType = reqType;
        }

        RequestType getReqType() const { return reqType; }

        const string& getDescription() const { return description; }
    };

    class ChainHandler {
        ChainHandler* nextChain;

        void sendRequestToNextHandler(const Request* req) {
            if (nextChain) {
                nextChain->handle(req);
            }
        }
    protected:
        virtual bool canHandleRequest(const Request& req) = 0;
        virtual void processRequest(const Request& req) = 0;

    public:
        ChainHandler() { nextChain = nullptr; }
        void setNextChain(ChainHandler* nextChain) { this->nextChain = nextChain; }

        void handle(const Request& req) {
            if (canHandleRequest(req)) {
                processRequest(req);
            } else {
                sendRequestToNextHandler(req);
            }
        }
    };

    class Handler1 : public ChainHandler {
    protected:
        bool canHandlerRequest(const Request& req) override {
            return req.getReqType() == RequestType::REQ_HANDLER_1;
        }

        void processRequest(const Request& req) override { ... }
    };

    class Handler2 : public ChainHandler {
    protected:
        bool canHandlerRequest(const Request& req) override {
            return req.getReqType() == RequestType::REQ_HANDLER_2;
        }

        void processRequest(const Request& req) override { ... }
    };

    class Handler3 : public ChainHandler {
    protected:
        bool canHandlerRequest(const Request& req) override {
            return req.getReqType() == RequestType::REQ_HANDLER_3;
        }

        void processRequest(const Request& req) override { ... }
    };

    int main (int argc, const char * argv[]) {
        Handler1 handler1;
        Handler2 handler2;
        Handler3 handler3;

        handler1.setNextChain(&handler2);
        handler2.setNextChain(&handler3);

        Request req("...", RequestType::REQ_HANDLER_3);
        handler1.handle(req);

        return 0;
    }
    ```

### 行为变化

- 命令模式

    命令模式将请求封装为一个对象，从而允许用户使用不同的请求对客户端进行参数化。

    ```cpp
    class Command {
    public:
        virtual void execute() = 0;
    };

    class ConcreteCommand1 : public Command {
        string args;

    public:
        ConcreteCommand1(const string& args) { this->args = args; }
        void execute() override { ... }
    };

    class ConcreteCommand2 : public Command {
        string args;
    public:
        ConcreteCommand2(const string& args) { this->args = args; }
        void execute() override { ... }
    };

    class MarcoCommand : public Command {
        vector<Command*> commands;
    public:
        void addCommand(Command* command) {
            commands.push_back(command);
        }

        void execute() override {
            for (auto& command : commands) {
                command->execute();
            }
        }
    };

    int main(int argc, const char* argv[]) {
        ConcreteCommand1 command1(receiver, "...");
        ConcreteCommand2 command2(receiver, "...");

        MacroCommand macro;
        macro.addCommand(&command1);
        macro.addCommand(&command1);

        macro.execute();
    }
    ```

- 访问者模式

    访问者模式旨在将数据结构与在该数据结构上的操作分离，从而使得添加新的操作变得更加容器，而无需修改数据结构本身。

    ```cpp
    // 元素抽象类
    class Element {
    public:
        virtual void accept(Visitor& visitor) = 0;
        virtual ~Element() {}
    };

    // 元素类实现
    class ElementA : public Element {
    public:
        void accept(Visitor& visitor) override {
            visitor.visitElementA(*this);
        }
    };

    class ElementB : public Element {
    public:
        void accept(Visitor& visitor) override {
            visitor.visitElementB(*this);
        }
    };

    // 访问者抽象类
    class Visitor {
    public:
        virtual void visitElementA(ElementA& element) = 0;
        virtual void visitElementB(ElementB& element) = 0;
    };

    // 访问者类实现
    class Visitor1 : public Visitor {
    public:
        void visitElementA(ElementA& element) override { ... }
        void visitElementB(ElementB& element) override { ... }
    };

    class Visitor2 : public Visitor {
    public:
        void visitElementA(ElementA& element) override { ... }
        void visitElementB(ElementB& element) override { ... }
    };

    int main(int argc, const char* argv[]) {
        Visitor2 visitor;
        ElementB elementB;
        elementB.accept(visitor);

        ElementA elementA;
        elementA.accept(visitor);

        return 0;
    }
    ```

### 领域问题

- 解释器模式

    解释器模式提供了评估语言的语法或表达式的方式。

    解释器模式对给定的一个语言，定义它的文法的一种表示方法，并定义一个解释器，该解释器用来解释语言中的句子。

    ```cpp
    class Expression {
    public:
        virtual int interpreter(map<char, int> var) = 0;
        virtual ~Expression() {}
    };

    class VarExpression : public Expression {
        char key;

    public:
        VarExpression(const char& key) {
            this->key = key;
        }

        int intrepreter(map<char, int> var) override {
            return var[key];
        }
    };

    class SymbolExpression : public Expression {
    protected:
        Expression* left;
        Expression* right;

    public:
        SymbolExpression(Expression* left, Expression* right) {
            this->left = left;
            this->right = right;
        }
    };

    class AddExpression : public SymbolExpression {
    public:
        AddExpression(Expression* left, Expression* right) : SymbolExpression(left, right) { ... }

        int interpreter(map<char, int> var) override {
            return left->interpreter(var) + right->interpreter(var);
        }
    };

    class SubExpression : public SymbolExpression {
    public:
        SubExpression(Expression* left, Expression* right) : SymbolExpression(left, right) { ... }

        int interpreter(map<char, int> var) override {
            return left->interpreter(var) - right->interpreter(var);
        }
    };

    Expression* analyse(string expStr) {
        stack<Expression*> expSt;
        Expression* left = nullptr;
        Expression* right = nullptr;

        for (int i = 0; i < expStr.size(); ++i) {
            switch(expStr[i]){
                case '+' : {
                    left = expStack.top();
                    right = new VarExpression(expStr[++i]);
                    expSt.push(new AddExpression(left, right));
                    break;
                }
                case '-': {
                    left = expStack.top();
                    right = new VarExpression(expStr[++i]);
                    expSt.push(new SubExpression(left, right));
                    break;
                }
                default:
                expStack.push(new VarExpression(expStr[i]));
            }
        }

        Expression* expression = expSt.top();
        return expression;
    }
    ```