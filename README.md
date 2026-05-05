# TODO
- [ ] save/load
- [ ] solver
- [ ] connect output of factory
- [x] finish fixes
- [ ] json should be smaller
- [ ] make more consistent ownership of subnodes and subfactories
- [ ] decide over how to print errors in the session manager, by *err or signals


## save file example
```json

{
    "metadata" : {
        <!-- some options and future stuff : { -->
        },
            "data" : {
                "root_factory" : [
                {
                    "index": 0
                        "name": "some name",
                    "machine": "constructor",
                    "type": "machine"
                        "machineLimit": 5,
                    "recipe": "someRecipe",
                    "ports" : {
                        "inputs": {
                            "index": 0,
                            "item": SOMEITEM
                                "connectedTo": "1x1"
                        },
                        "outputs": {
                            "index": 1,
                            "item": SOMEITEM
                                "connectedTo": null
                        }
                    }
                },
                {
                    "index": 1
                        "name": "some other name",
                    "machine": "assebler",
                    "type": "machine"
                        "machineLimit": 5,
                    "recipe": "someRecipe",
                    "ports" : {
                        "inputs": {
                            "index": 0,
                            "item": SOMEITEM
                                "connectedTo": null
                        },
                        "outputs": {
                            "index": 1,
                            "item": SOMEITEM
                                "connectedTo": "0x0"
                        }
                    }
                },
                {
                    "index": 2
                        "name": "some factory",
                    "type": "factory"
                        "ports" : {
                            "inputs": {
                                "index": 0,
                                "item": SOMEITEM
                                    "connectedTo": null
                            },
                            "outputs": {
                                "index": 1,
                                "item": SOMEITEM
                                    "connectedTo": null
                            }
                        },
                    "factory" : [
                    {
                        "index": 0
                            .....
                    }
                    ]
                },
                {
                }
                ]
            }
    }

```



