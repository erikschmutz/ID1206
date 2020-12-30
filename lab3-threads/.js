class Observer {


    constructor() {
        this.observers = []
        this.condition = 100;
    }

    addObserver(object) {
        if (object.onEvent) {
            throw new Error("You need an onEvent class")
        }
    }
}

