# Introduction
The eau2 system is a distributed key-value store for big data analysis. There will be applications that can interact with the store in order to get and put data from different nodes that may not fit into memory. The system can read in large chunks of data and create different types of data structures depending on the use case. It can also serialize and deserialize data in order to distribute data between nodes.

# Architecture
There are 3 levels to the eau2 system: the key-value store and networking layer, the data abstraction layer, and the application layer.

Level 1: KV Store and Network Layer
The first and lowest layer deals with the distributed KV store. A KV store runs on each node, creating a distributed system. Each KV store on each node has part of the data, and the nodes talk to each other to exchange data when needed. The networking portion of this layer helps the nodes talk to each other and distribute data when needed. There is a master node that helps tell each node which nodes exist via their IP. The children nodes can pass data around to each other easily (since all the data has already been serialized) after registering with the master node.

Level 2: Data Abstraction Layer
This level provides an abstraction of the data that exists in Level 1. In this layer, data structures exist in a user friendly format, representing the serialized data from Level 1. Level 2 interacts with Level 1 through a KDStore which contains a KVStore. It adds data to the KV Store by serializing it. Any data that will be put in the KV store must first be put into a DataFrame.

Level 3: Application Layer
This layer is where we can do ML and AI and GPUs! Any use case / algorithm that requires data can be created as an application in this layer. It will interact with Layer 2 to manipulate the data. Layer 2 interacts with Layer 1 to take care of distributed data.

# Implementation
## Level 1 implementation:

### Milestone 1:
The Key-value store is basically a map of keys to values. Therefore we plan on implementing a KVStore class with a Map field.

Pseudocode for implementing the KVStore:

```

class KVStore {
    Map map;

    waitAndGet(k) => v
    put(k, v) => void
}
class Key {
    String name;
    size_t home;
}
class Value {
    char* data;
}

```
The KVStore class has a waitAndGet method, which takes a key and returns the data value. Since a key has a "home" and knows on which node it lives, we can get the value of the data there by using our network implementation. So the node requesting data would send a message to the "home" node (the one that has the key's value), and that "home" node would send back the value. In the implementation for the network portion of this, the messages sent between nodes will have specific types / be prefaced with specific words that indicate what kind of operation is happening (registering, asking for data, etc...).
The put(k, v) method would call the Map's put and store data in the same way a map stores data.

The network portion of this layer will consist of a client.cpp and a server.cpp. A client / node has a thisNode() method that returns a number, which is the number that represents which node it is in the network. This number corresponds to the "home" size_t field that exists on a Key.
Each client node has its own KVStore. Because it goes through the registration period with the server node, it knows who all the other nodes are in the system.

### Milestone 2:
We properly implemented the classes outlined in milestone 1 (KVStore, Key, Value).
We also added a KDStore which directly interfaces with the dataframe classes. It has a KVStore as a field which strictly handles serialized data. This way the layers of passing data are separated.

### Milestone 3:
We added a Lock as a field to KVStore to allow for threading. We also made our Application class inherit from Thread to implement threading.
We also properly implemented waitAndGet on the KVStore now that we added threading to our program.

### Milestone 4:
We started with the PseudoNetwork code for this, and attempted to get it to work. There is a lot of debugging we are sifting through, but we implemented the logic of distributing data over the pseudonetwork. Each application inherits from the Thread class, and each application is spun up in the main method. The KVStore also now holds the network object so that it can interface with the network. 

## Level 2 implementation:

### Milestone 1:
Since data will be put into the KVStore as a serialized DataFrame, we have methods that convert data from one form into a DataFrame. For example, the  methods on datastructures fromArray, fromScalar, etc... convert an Array of data and a Scalar of data into a DataFrame, respectively. We can expand these to also include other datatypes.
The classes that we will include here are all our serialization classes: Serialize.h and Deserialize.h. In addition, we will have the DataFrame and all classes associated with the DataFrame (such as Schema, Lists, Columns, Rows, Rowers, and Fielders, etc...).

### Milestone 2:
We implemented serialization for the dataframe and related classes. 
We also implemented fromArray. We will probably be able to implement new methods like fromScalar, etc. more easily now. 

### Milestone 3:
We implemented fromScalar in order to get the example given from class to run.

### Milestone 4:
We refactored a lot of code for this milestone. We cleaned up our array classes as well as columns. Our columns now hold keys which point to chunks of data, which are our newly refactored Array classes. This helped us with the "distributed" portion of the assignment. We also did this so that the DataFrame API would not have to change too much because the column classes took care of the interfacing with the KVStore. We added all the classes given to us for the WordCounter example.

## Level 3 implementation:
Because Level 3 is the layer where the user writes code that sits on top of the first two players, Level 3 is mostly use cases, which can be seen in the section below.

### Milestone 3:
We copied in the code suggested to get to run and ran it with threads to prepare for the network section of Milestone 4.

### Milestone 4:
We now have a main method which is used to spin up multiple WordCounters running on different threads. The WordCount API itself deals with mocking the nodes and kicking off each application. 

# Use cases
The example of the system that computes a sum and checks the computation is one use case.

Other use cases could also have similar math operations. For example, any type of filter or map method can be implemented on the data. The problem statement of 'find me all broken pPhones that were assembled by workers between 7 and 17 years old' is a very specific fitlering use case that can be run on large amounts of data.

## Milestone 2:
A successful use case is summing/subtracting ints. It was the trivial example given to us in the assignment.

## Milestone 3:
A successful use case is what was given to us to run for this assignment. This use case can be seen in the Demo.h file under src/application_layer

## Milestone 4:
The WordCount is a use case for this. 

# Open questions

## Milestone 4
- Passing NetworkIfc by reference?? How to set up the constructor of the Application class? Did we do this correctly?
- Is this filereader supposed to be for data that doesn't fit into memory?
- We are having issues figuring out the node index in the column class when we need the kv store to get us data. Is the node index something we should pass around? We tried grabbing the thread id and looking it up in the thread_id to node_num map that the Network holds, but for some reason we were on a thread which was not the same one that was registered. We also feel like we are passing around the kv store everywhere, is that something that makes sense?

# Status
We worked a lot on refactoring code this week to make chunks of data. We could probably clean up some of the logic in the Column classes, but due to time constraints, we chose to move forward with debugging since we had already spent a lot of time refactoring this week. We have the code/logic written for implementing the WordCounter with a PseudoNetwork, but have been making our way through bugs with what we have set up. We currently are running a very simple example of just one node and have issues with data retrieval. It seems as though data is being put in our kvstore just fine, but perhaps have a serialization issue or some confusion with types which is preventing us from retrieving data properly from the kv store.

We have learned a LOT about refactoring and trying to balance that time :) We also got burned a bit this week by not testing in smaller chunks as we implemented the pseudonetwork classes. We didn't really know to test in smaller chunks without setting up the entire system, but maybe we could have done more with testing serialization/deserialization of classes, although it felt like there wasn't much to test on a smaller scale before we implemented the whole thing. But spending a whole day slogging through bugs isn't fun either. ¯\_(ツ)_/¯ We have learned a lot and time management is hard.

We left print statements in our code to show how far it gets. It can be run using: ```make ; make run```

- ** NOTE: We commented out the consumption of val_ in get() in string.h. Now you can call get() as many times as you want. Didn't want to waste time debugging right now. **

## Work that remains:
- Continue fixing valgrind errors
- Get rid of fielder and rower - not needed anymore - do this after we figure out the valgrind issues (was getting messed up when we tried to change Schema in Row)
- NEED TO CLEAN UP fromVisitor()
- Update tests - many don't work anymore with certain changes to columns now holding keys. 
- Finish implementing the network - fix bugs
- Need to clean up Column class - can easily make this look like BaseArray

## Milestone 5 Notes/Work Completed:
- Commented out set in DF and Columns - no longer needed
- Assumption we have made: When we receive data from a different node, we store it in our kvstore at the old key that is given to us. We don't think it really makes sense that the data should be stored at a new key that reflects `this node` since then it gets confusing as to who actually owns the data (the other node or this node?) We also don't want to reset the key to this node because eventually this data will be stored in a cache, and if we flush the cache we will toss all the data as it will no longer live on its original node. Our put_ method in kvstore then takes a parameter that determines if it was initially distributed by node 0. If distributed by node 0 (in the beginning in fromVisitor) then there is a check for the home of the key so that the node knows it should put the data on itself. Otherwise, it was data received by another node as the return for waitAndGet and hence should just be added to this node's kvstore at its original key. 

## Questions:
- How do we set this up with multiple terminal windows? Are we doing this right? Did it based off of last networking assignment
    - When to call wordcount to start? Do we need to sleep it or make it wait for some time?
    - show the main methods for server, client, and main
- Doesn't server need an IP address? 

- Serialization for messages?
- Should all the clients and server both connect to 8080? 
- for(size_t i = 0; i < sizeof(ports_) / sizeof(size_t); i++)

------ 
- Will we need a teardown method/how will that get set up?
- Ask about passing current node_num around?
