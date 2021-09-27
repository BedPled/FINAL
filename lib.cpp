// ПО фондовый рынок

// паттерн посредник
/*
    Покупатель акций
    Компания
    Государство
    Биржа
    Акции
    Облигации
 */
#include <vector>
#include <stdexcept>

class Owner; // владелец
class Security; // ценная бумага

class Share;
class Bond;


class Company;
class State;
class Player;

class StockExchange;

// -----------------------------------------------------виды объектов---------------------------------------------------
/**
 * Интерфейс Посредника предоставляет метод, используемый компонентами для
 * уведомления посредника о различных событиях. Посредник может реагировать на
 * эти события и передавать исполнение другим компонентам.
 */
class BaseComponent;
class Mediator {  // ПОСРЕДНИК
public:
    virtual void Notify(BaseComponent *sender, std::string event) const = 0;
};

/**
 * Базовый Компонент обеспечивает базовую функциональность хранения экземпляра
 * посредника внутри объектов компонентов.
 */
class BaseComponent {
protected:
    Mediator *mediator_;

public:
    BaseComponent(Mediator *mediator = nullptr) : mediator_(mediator) {
    }
    void set_mediator(Mediator *mediator) {
        this->mediator_ = mediator;
    }
};

class Owner : public BaseComponent { // владелец ценной бумаги
    int budget;
public:
    std::vector<Security> securities;
    Owner(int budget) {
            if(budget < 0) {
                throw std::logic_error("Некорректный бюджет");
            } else this->budget = budget;
    }

    virtual void buy(unsigned int n) {};
    virtual void sell(unsigned int n, unsigned  int cost) {};


};

class Security { // ценная бумага
protected:
    std::string ticket;
    int cost;
    Owner owner;

public:
    Security(const std::string &ticket, int cost, const Owner &owner) : ticket(ticket), cost(cost), owner(owner) {}

    const std::string &getTicket() const {
        return ticket;
    }

    int getCost() const {
        return cost;
    }

    const Owner &getOwner() const {
        return owner;
    }

    void setCost(int cost) {
        if (cost < 0) {
            throw std::logic_error("Цена на акции отрицательная");
        } else Security::cost = cost;
    }

    void setOwner(const Owner &owner) {
        Security::owner = owner;
    }
};
// ---------------------------------------------объекты для обмена------------------------------------------------------
class Share : public Security { // акция
    // имеет стоимость
    // может принадлежать игроку/компании/etf

    // создаётся компанией
public:
    Share(const std::string ticket, unsigned int cost, const Owner owner) : Security(ticket, cost, owner) {}
};

class Bond : public Security  { // облигация
    int percent; // годовой процент наценки который выплатит компания от 1 до 100 (константа во времение) +- 7%
public:
    Bond(const std::string &ticket, int cost, const Owner &owner, int percent) : Security(ticket, cost, owner),
                                                                                 percent(percent) {}
};

// --------------------------------------------------особый игрок-------------------------------------------------------
//class ETF :  public Owner { // ETF
//    int percent; // годовой процен
//    int price;// имеет стоимость как владелец
//
//    // содержит акции как владелец
//    void sell(unsigned int n, unsigned  int cost) override {
//        std::cout << "is sell" << std::endl;
//        this->mediator_->Notify(this, "sell");
//    }
//
//    void buy(unsigned int n, std::string ticket) override {
//        std::cout << "is buy" << std::endl;
//        this->mediator_->Notify(this, "buy");
//    }
//
//    void IPO (unsigned int n) { // // может обратиться на биржу чтобы разместить там акции
//        std::cout  << "is IPO" << std::endl;
//        this->mediator_->Notify(this, "IPO");
//    }
//
//    // создаёт свои собственные акции как компания
//};

// -----------------------------------------------ключевые игроки-------------------------------------------------------
/**
 * Конкретные Компоненты реализуют различную функциональность. Они не зависят от
 * других компонентов. Они также не зависят от каких-либо конкретных классов
 * посредников.
 */
//class Component1 : public BaseComponent {
//public:
//    void DoA() {
//        std::cout << "Component 1 does A.\n";
//        this->mediator_->Notify(this, "A");
//    }
//    void DoB() {
//        std::cout << "Component 1 does B.\n";
//        this->mediator_->Notify(this, "B");
//    }
//};

class Company : public Owner { // компания
    std::string name;
    int countShare = 0; // кол-во акций
public:
    void sell(unsigned int n, unsigned  int cost) override {
        std::cout << name << "is sell" << n << "shares" << std::endl;
        this->mediator_->Notify(this, "sell");
    }

    void buy(unsigned int n) override {
        std::cout << name << "is buy" << std::endl;
        this->mediator_->Notify(this, "buy");
    }

    void IPO (unsigned int n, std::string ticked, unsigned cost) { // // может обратиться на биржу чтобы разместить там акции
        std::cout << name << "is IPO " << n << "shares"<< std::endl;
        countShare += n;
        for (int i = 0; i < n; ++i) {
            Share s(ticked, cost, *this);
            securities.push_back(s);
        }

        this->mediator_->Notify(this, "IPO");
    }

    void pay (int N) { // может выплачивать девиденты
        std::cout << name << "is pay " << N << "  dollars " << std::endl;
        this->mediator_->Notify(this, "pay");
    }

    const std::string &getName() const {
        return name;
    }

    int getCountShare() const {
        return countShare;
    }
};

class State : public Owner { // государство

    int budget; // имеет бюджет
public:
    std::vector<Bond> bonds;

    void IPO (unsigned int n) { // выпускает облигации
        std::cout <<"State is make "<< n <<" bonds" << std::endl;
        for (int i = 0; i < n; ++i) {
            Bond b("ST", 1000, *this, 7);
            bonds.push_back(b);
        }
        this->mediator_->Notify(this, "bonds");
    }
    // выпускает облигации

    void addMoney () { // берёт нологи с дохода
        std::cout <<"State is take tax" << std::endl;
        this->mediator_->Notify(this, "take tax");
    }
};

class Player : public Owner { // игрок на бирже

    // имеет бюджет
    // имеет ЦБ

    // может покупать/продавать акции у брокера
public:
    void sell(unsigned int n, unsigned  int cost) override {
        std::cout  << "sell 1" << std::endl;
        this->mediator_->Notify(this, "sell");
    }

    void buy(unsigned int n) override {
        std::cout << "buy 1" << std::endl;
        this->mediator_->Notify(this, "buy");
    }

};

// ---------------------------------------посредники--------------------------------------------------------------------

/**
 * Конкретные Посредники реализуют совместное поведение, координируя отдельные
 * компоненты.
 */

//class ConcreteMediator : public Mediator {
//private:
//    Component1 *component1_;
//    Component2 *component2_;
//
//public:
//    ConcreteMediator(Component1 *c1, Component2 *c2) : component1_(c1), component2_(c2) {
//        this->component1_->set_mediator(this);
//        this->component2_->set_mediator(this);
//    }
//
//    void Notify(BaseComponent *sender, std::string event) const override {
//        if (event == "A") {
//            std::cout << "Mediator reacts on A and triggers following operations:\n";
//            this->component2_->DoC();
//        }
//        if (event == "D") {
//            std::cout << "Mediator reacts on D and triggers following operations:\n";
//            this->component1_->DoB();
//            this->component2_->DoC();
//        }
//    }
//};


class StockExchange : public Mediator { // биржа // посредник


    Company *company_;
    State *state_;
    Player *player_;

public:
    StockExchange(Company *company, State *state, Player *player) : company_(company), state_(state), player_(player) {

        this->company_->set_mediator(this);
        this->state_->set_mediator(this);
        this->player_->set_mediator(this);

    }

    void Notify(BaseComponent *sender, std::string event) const override {
        // компания
        if (event == "IPO") {
            this->player_->buy(1);
        }

        if (event == "bonds") {
            this->player_->buy(1);
        }


        if (event == "take tax") {
            this->company_->pay(5);
        }

        if (event == "sell") {
            this->state_->addMoney();
        }

        if (event == "buy") {
            // торги с биржей
            company_->sell(1, 100);
        }

    }

};


// ---------------------------------------------------------------------------------------------------------------------


