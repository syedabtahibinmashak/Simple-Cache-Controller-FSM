#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

enum State 
{
    IDLE,
    CHECK_CACHE,
    READ_MEMORY,
    WRITE_MEMORY,
    UPDATE_CACHE,
    DONE
};

struct Request 
{
    string type; 
    int address, data;
};

class Memory 
{
    public:

    unordered_map<int, int> mem;
    
    int read(int address) 
    {
        cout <<"          Reading address " <<address <<"..." <<endl;
        return mem[address];
    }
    
    void write(int address, int data) 
    {
        cout <<"          Writing " <<data <<" to address " <<address <<"..." <<endl;
        mem[address] = data;
    }
};

class CacheController 
{
    private:
    
    State currentState;
    unordered_map<int, int> cache;
    Request currentRequest;
    Memory &memory;
    bool cacheHit;
    int tempData;

    bool isCacheHit(int address) 
    {
        return cache.find(address) != cache.end();
    }

    public:

    CacheController(Memory &mem) : memory(mem)
    {
        currentState = IDLE;
    }

    void processRequest(Request req) 
    {
        currentRequest = req;
        currentState = IDLE;

        cout <<endl <<"==============================================" <<endl;
        cout <<"Processing Request: " <<req.type <<" Address: " <<req.address;
        if (req.type == "WRITE") cout <<" Data: " <<req.data;
        cout <<endl;

        while (currentState != DONE) 
        {
            switch (currentState) 
            {

                case IDLE:

                    cout <<"[=STATE=] IDLE -> CHECK_CACHE" <<endl;
                    currentState = CHECK_CACHE;

                    break;

                case CHECK_CACHE:

                    cacheHit = isCacheHit(currentRequest.address);
                    if (cacheHit) 
                    {
                        cout <<"[=STATE=] CHECK_CACHE -> HIT" <<endl;
                        if (currentRequest.type == "READ") 
                        {
                            tempData = cache[currentRequest.address];
                            if(tempData == 0) cout <<"          No valid Data!" <<endl;
                            else cout <<"          Cache Hit! Data = " <<tempData <<endl;
                            currentState = DONE;
                        } 
                        else if (currentRequest.type == "WRITE")
                        {
                            cout <<"          Cache Hit! Updating cache" <<endl;
                            cache[currentRequest.address] = currentRequest.data;
                            currentState = WRITE_MEMORY;
                        }
                    } 
                    else 
                    {
                        cout <<"[=STATE=] CHECK_CACHE -> MISS" <<endl;
                        if (currentRequest.type == "READ") currentState = READ_MEMORY; 
                        else if (currentRequest.type == "WRITE") currentState = WRITE_MEMORY;
                    }

                    break;

                case READ_MEMORY:

                    cout <<"[=STATE=] READ_MEMORY" <<endl;
                    tempData = memory.read(currentRequest.address);
                    currentState = UPDATE_CACHE;

                    break;

                case WRITE_MEMORY:

                    cout <<"[=STATE=] WRITE_MEMORY" <<endl;
                    memory.write(currentRequest.address, currentRequest.data);
                    currentState = UPDATE_CACHE;

                    break;

                case UPDATE_CACHE:

                    cout <<"[=STATE=] UPDATE_CACHE" <<endl;
                    if (currentRequest.type == "READ") 
                    {
                        cache[currentRequest.address] = tempData;
                        cout <<"          Cache updated with data = " <<tempData <<endl;
                    } 
                    else if (currentRequest.type == "WRITE")
                    {
                        cache[currentRequest.address] = currentRequest.data;
                        cout <<"          Cache updated with data = " <<currentRequest.data <<endl;
                    }
                    currentState = DONE;
                    
                    break;

                default:
                    break;
            }
        }

        cout << "[=STATE=] DONE" <<endl;
    }               
};

int main() 
{

    Memory memory;
    CacheController cache(memory);
    queue<Request> cpuQueue;

    while (true) 
    {
        cout <<endl <<"==================== MENU ====================" <<endl;
        cout <<"1. Add READ request" <<endl;
        cout <<"2. Add WRITE request" <<endl;
        cout <<"3. Process requests" <<endl;
        cout <<"4. Exit Program" <<endl;
        
        cout <<"Enter choice: "; int choice; cin >>choice;

        if (choice == 1) 
        {
            cout <<"Enter address: "; int addr; cin >>addr;
            cpuQueue.push({"READ", addr, 0});
            cout <<"READ request added!" <<endl;
        }

        else if (choice == 2) 
        {
            cout <<"Enter address: "; int addr; cin >>addr;
            cout <<"Enter data: "; int data; cin >>data;
            cpuQueue.push({"WRITE", addr, data});
            cout <<"WRITE request added!" <<endl;
        }

        else if (choice == 3) 
        {
            if (cpuQueue.empty()) cout <<"Queue is empty!" <<endl;
            else 
            {
                while (!cpuQueue.empty()) 
                {
                    Request req = cpuQueue.front(); cpuQueue.pop();
                    cache.processRequest(req);
                }
            }
        }

        else if (choice == 4) 
        {
            cout <<endl <<"============= PROGRAM TERMINATED =============" <<endl;
            break;
        }

        else cout <<"Invalid choice!" <<endl;
    }

    return 0;
}