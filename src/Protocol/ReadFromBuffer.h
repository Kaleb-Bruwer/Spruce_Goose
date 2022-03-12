#pragma once

class ReadFromBuffer{
public:
    template<typename T>
    inline static void switchEndian(void* iterator){
        char buffer[sizeof(T)];

        for(int i=0; i<sizeof(T); i++){
            buffer[sizeof(T) - i - 1] = ((char*)iterator)[i];
        }
        memcpy(iterator, &(buffer[0]), sizeof(T));

        // if(sizeof(T) > 64){
        //     char* temp = new char[sizeof(T)];
        //     for(int i = 0; i<sizeof(T); i++){
        //         temp[sizeof(T)-i-1] = ((char*)iterator)[i];
        //     }
        //     memcpy(iterator,temp, sizeof(T));
        //     delete [] temp;
        // }
        // else{
        //     char buffer[64];
        //     for(int i=0; i<sizeof(T); i++){
        //         buffer[sizeof(T) - i - 1] = ((char*)iterator)[i];
        //     }
        //     memcpy(iterator, &(buffer[0]), sizeof(T));
        // }
    };

    template<typename T>
    inline static T read(char* buffer, int& index, int bufferSize){
        if(bufferSize - index < sizeof(T)){
            throw(0);
        }

        T result;
        memcpy(&result, &(buffer[index]),sizeof(T));
        switchEndian<T>(&result);
        index += sizeof(T);
        return result;
    }
};
