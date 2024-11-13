#include <iostream>
#include <string>
using namespace std;
class VendingMachine;


//
//
//Product클래스
class Product {
protected:
    static int nextId;
    int id;
    string p_name;
    double p_price;
    double p_calorie;

public:
    Product(string name, double price, double p_calorie)
        : p_name(name), p_price(price), p_calorie(p_calorie), id(nextId++) {
        cout << "[Constructor] Product created: " << p_name << " (ID: " << id
            << ", Price: $" << p_price << ")" << endl;
    }
    virtual int getId() const { return id; }

    virtual ~Product() {
        cout << "[Destructor] Product destroyed: " << p_name << " (ID: " << id
            << ")" << endl;
    }
    virtual string getName() const { return p_name; }
    virtual double getPrice() const { return p_price; }

    virtual void describe() const {
        cout << "Product: " << p_name << " (ID: " << id << ", Price: $" << p_price
            << endl;
    }
};
int Product::nextId = 1;


class Beverage : public Product {
public:
    Beverage(string name, double price, double calorie)
        : Product(name, price, calorie) {
        cout << "Beverage created: " << name << " (" << calorie << " calories)\n";
    }

    void describe() const override {
        cout << "Beverage: " << getName() << " (ID: " << getId()
            << ", Price: $" << getPrice() << ", Calorie: " << this->p_calorie <<
            "calories)\n";
    }
};

class Snack : public Product {
public:
    Snack(string name, double price, double calorie)
        : Product(name, price, calorie) {
        cout << "Snack created: " << name << " (" << calorie << " calories)\n";
    }

    void describe() const override {
        cout << "Snack: " << getName() << " (ID: " << getId()
            << ", Price: $" << getPrice() << ", Calories: " << this->p_calorie <<
            ")\n";
    }
};

//
//
//State클래스
class State {
protected:
    string s_name;
    VendingMachine* machine;

public:
    State(VendingMachine* machine)
        : machine(machine) {
        cout << "[Constructor] Constructing State: "<< endl;
    }

    virtual ~State() {
        cout << "[Destructor] Destructing State: " << endl;
    }
    virtual string getName() const = 0;
    virtual void insertCoin(double coin) = 0;
    virtual void ejectCoin() = 0;
    virtual void dispense(string productName) = 0;
};
//
//
//VendingMachine클래스
class VendingMachine {
private:
    State* noCoinState;
    State* hasCoinState;
    State* soldOutState;

    State* currentState;
    const int MAX_NUM_PRODUCT = 10;
    Product* inventory[10];
    int num_of_products; //재고 개수
    bool hasCoin;
    double coinValue;

    void printState(string action) const {
        cout << "Action: " << action << " | Current State: " << currentState->
            getName() << " | Coin Value: " << this->coinValue << "\n";
    }

public:
    VendingMachine();
    ~VendingMachine() {
        cout << "[Destructor] Destructing VendingMachine" << endl;
        delete noCoinState, delete hasCoinState, delete soldOutState;
        for (int x = 0; x < num_of_products; x++) {
            delete inventory[x];
        }
    }


    void setState(State* state) {
        currentState = state;
        printState("State Changed");
    }

    void insertCoin(double coin);

    void ejectCoin();

    void dispense(string productName);

    Product* removeProduct(string productName) {
        for (int x = 0; x < num_of_products; x++) {
            if (inventory[x] != nullptr && inventory[x]->getName()
                == productName) {

                Product* removingProduct = inventory[x];
                for (int y = x; y < num_of_products - 1; y++) {
                    inventory[y] = inventory[y + 1];
                }

                inventory[num_of_products - 1] = nullptr;
                num_of_products += -1;
                return removingProduct;
            }
        }
        return nullptr;
    }


    void addProduct(Product* product) {


        for (int x = 0; x < MAX_NUM_PRODUCT; x++) {
            if (inventory[x] == nullptr) {
                inventory[x] = product;
                num_of_products += 1;


                if (currentState == soldOutState) {
                    setState(noCoinState);
                }
                return;
            }
        }
    }


    bool isProductAvailable(string productName) const {
        for (int x = 0; x < num_of_products; x++) {
            if (inventory[x] != nullptr && inventory[x]->getName()
                == productName) {
                return true;
            }
        }
        return false;
    }


    double getProductPrice(string productName) const {
        for (int x = 0; x < num_of_products; x++) {
            if (inventory[x] != nullptr && inventory[x]->getName()
                == productName) {
                return inventory[x]->getPrice();
            }
        }
        return 0.0;
    }


    void displayInventory() const {
        int countDisplay = 0;
        cout << "Current Inventory:" << endl;
        for (int x = 0; x < MAX_NUM_PRODUCT; x++) {
            if (inventory[x] != nullptr) {
                cout << "Product: " << inventory[x]->getName()
                    << " (ID: " << inventory[x]->getId()
                    << ", Price: $" << inventory[x]->getPrice()
                    << ")" << endl;
                countDisplay += 1;
            }
            if (countDisplay == num_of_products) {
                break;
            }
        }
        cout << "Total items: " << num_of_products << endl;
    }

    int getInventoryCount() const { return num_of_products; }

    State* getNoCoinState() const { return noCoinState; }
    State* getHasCoinState() const { return hasCoinState; }
    State* getSoldOutState() const { return soldOutState; }

    bool hasInsertedCoin() const { return hasCoin; }
    void setCoinInserted(bool inserted) { hasCoin = inserted; }
    double getCoinValue() const { return coinValue; }
    void resetCoinValue() { coinValue = 0.0; }

    void addCoinValue(double coin) { coinValue += coin; }
};

//NoCoinState Class
class NoCoinState : public State {
public:
    NoCoinState(VendingMachine* t_machine) : State(t_machine) {
        s_name = "No Coin";
        cout << "[Constructor] Constructing State: " << s_name << endl;
    }

    ~NoCoinState() override {
        cout << "[Destructor] Destructing State: " << s_name << endl;
    }
    void insertCoin(double coin) override {//main implementation
        machine->addCoinValue(coin);
        machine->setState(machine->getHasCoinState());
        cout << "Coin inserted: " << coin << endl;
    }
      
    void ejectCoin() override {
        cout << "No coin to eject" << endl;
    }
    void dispense(string productName) override {
        cout << "Insert coin in order to dispense" << endl;
    }
    string getName() const override { return this->s_name; }
};

//HasCoinState class
class HasCoinState : public State {
public:
    HasCoinState(VendingMachine* t_machine) :State(t_machine) {
        s_name = "Has Coin";
        cout << "[Constructor] Constructing State: " << s_name << endl;
    }
    ~HasCoinState() override {
        cout << "[Desturctor] Destructing State: " << s_name << endl;
    }
    void insertCoin(double coin) override {
        machine->addCoinValue(coin);
        cout << "Coin inserted again: " << coin << endl;
    }
    void ejectCoin() override {
        machine->resetCoinValue();
        machine->setState(machine->getNoCoinState());
        cout << "Coin ejected" << endl;
    }
    string getName() const override { return this->s_name; }
    //main
    void dispense(string productName) override { //재고 확인->금액 확인->물품 빼기
        //재고확인
        if (!machine->isProductAvailable(productName)) {
            cout << "Product not available: " << productName << endl;
            machine->setState(machine->getSoldOutState());
            return;
        }
        double price = machine->getProductPrice(productName);

        if (machine->getCoinValue() < price) {
            cout << "Insufficient funds. Please insert more coins." << endl;
            cout << "Current balance : $" << machine->getCoinValue()
                << ", Required: $" << price << endl;
            return;
        }
        Product* product = machine->removeProduct(productName);
        machine->addCoinValue(-price);
        
        if (machine->getCoinValue() > 0) {
            cout << "Change returned: $" << machine->getCoinValue() << endl;
            machine->resetCoinValue();
        }

        if (machine->getInventoryCount() == 0) {
            machine->setState(machine->getSoldOutState());
        }
        else if (machine->getCoinValue() == 0) {
            machine->setState(machine->getNoCoinState());
        }
        else {
            machine->setState(machine->getHasCoinState());
        }

        delete product;
        
    }
};

class SoldOutState : public State {
public:
    SoldOutState(VendingMachine* t_machine) :State(t_machine) {
        s_name = "Sold Out";
        cout << "[Constructor] Constructing State : " << s_name << endl;
    }
    ~SoldOutState() override {
        cout << "[Destructor] Destructing State: " << s_name << endl;
    }
    void insertCoin(double coins) override {
        cout << "Machine Sold Out. Don't insert more coins." << endl;
    }
    void ejectCoin() override {
        cout << "No coins to eject" << endl;
    }
    void dispense(string productName) override {
        cout << "No product to dispense" << endl;
    }
    string getName() const override { return this->s_name; }
};

VendingMachine::VendingMachine() {
    cout << "[Constructor] Constructing VendingMachine" << endl;
    noCoinState = new NoCoinState(this);
    hasCoinState = new HasCoinState(this);
    soldOutState = new SoldOutState(this);
    //init
    currentState = noCoinState;
    num_of_products = 0;
    coinValue = 0.0;
    for (int x = 0; x < MAX_NUM_PRODUCT; x++) {
        inventory[x] = nullptr;
    }
    printState("Initialization");
}

void VendingMachine::insertCoin(double coin) {
    currentState->insertCoin(coin);
    printState("Insert Coin");
}
void VendingMachine::ejectCoin() {
    currentState->ejectCoin();
    printState("Eject Coin");
}
void VendingMachine::dispense(string productName) {
    currentState->dispense(productName);
    printState("Dispense");
}





int main() {
    cout << "==========Part 1==========" << endl;
    VendingMachine machine; //Create a VendingMachine instance
    Product* p_cola = new Beverage("Cola", 1.50, 330); //Create a new product
    Product* p_chips = new Snack("Chips", 1.00, 150); //Create a new product
    Product* p_water = new Beverage("Water", 1.00, 0); //Create a new product
    
    cout << "==========Part 2==========" << endl;
    //Add three products
    machine.addProduct(p_cola);
    machine.addProduct(p_chips);
    machine.addProduct(p_water);
    //Display inventory
    machine.displayInventory();
    /*
    cout << "==========Part 3==========" << endl;
    //Insert coin and dispense
    machine.insertCoin(1.00);
    machine.insertCoin(0.50);
    machine.dispense("Cola");
    cout << "==========Part 4==========" << endl;
    machine.insertCoin(1.00);
    machine.dispense("Water");
    cout << "==========Part 5==========" << endl;
    machine.insertCoin(0.50);
    machine.dispense("Chips"); // Should say insufficient funds
    machine.insertCoin(0.50);
    machine.dispense("Chips");
    cout << "==========Part 6==========" << endl;
    machine.insertCoin(1.00);
    machine.dispense("Candy"); // Should say product not available
    machine.displayInventory();
    cout << "==========Part 7==========" << endl;
    Product* p_chocolate = new Snack("Chocolate", 1.25, 200);
    Product* p_juice = new Beverage("Orange Juice", 1.75, 250);
    machine.addProduct(p_chocolate);
    machine.addProduct(p_juice);
    machine.displayInventory();
    cout << "==========Part 8==========" << endl;
    machine.insertCoin(2.00);
    machine.dispense("Chocolate");
    machine.insertCoin(2.00);
    machine.dispense("Orange Juice");
    machine.displayInventory();
    return 0;
    */
}
