#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>


template<typename T>
class Allocator{
public:
    using value_type = T;
private:
    struct Chunk{
        Chunk* next_ = nullptr;
    };
    struct Pool{
        int PoolSize_;
        int ChunkSize_;
        Chunk* head_;
        T* ptr_;

        Pool(): PoolSize_(0), ChunkSize_(0), head_(nullptr), ptr_(nullptr){}

        Pool(const Pool& src) = delete;

        Pool& operator=(const Pool& src) = delete;

        Pool(Pool&& other)  noexcept :
                head_(std::__exchange(other.head_, nullptr)),
                ptr_(std::__exchange(other.ptr_, nullptr)),
                PoolSize_(std::move(other.PoolSize_)),
                ChunkSize_(std::move(other.ChunkSize_)) {}

        Pool(const std::pair<int, int>& parametres) {
            ChunkSize_ = std::max(parametres.first, (int)((8/sizeof(T)) + 1));
            PoolSize_ = parametres.second;
            ptr_ = reinterpret_cast<T*> (malloc(PoolSize_ * ChunkSize_ * sizeof(T)));
            head_ = reinterpret_cast <Chunk*>(ptr_);
            Chunk* temp = head_;
            for(int i = 0; i < PoolSize_ - 1; ++i){
                temp->next_ = reinterpret_cast<Chunk*> (reinterpret_cast<T*> (temp) + ChunkSize_);
                temp = temp->next_;
            }
            temp->next_ = nullptr;
        }

        ~Pool(){
            free(ptr_);
        }
    };

private:
    std::shared_ptr<std::vector<Pool>> pools_ = nullptr;

public:

    explicit Allocator(std::vector<std::pair<int, int>>& PoolParametres) {
        pools_ = std::make_shared<std::vector<Pool>>();
        pools_->reserve(PoolParametres.size());
        std::sort(PoolParametres.begin(), PoolParametres.end());
        for(auto Parametre : PoolParametres) {
            (*pools_).push_back(std::move(Parametre));
        }
    }
    Allocator(const Allocator& src){
        pools_ = src.pools_;
    }
    Allocator& operator=(const Allocator& src){
        if (this == src) return *this;
        pools_ = src.pools_;
        src.pools_ = nullptr;
        return *this;
    }

    T* allocate(size_t size) {
        for(int i = 0; i < (*pools_).size(); ++i ){
            if(size > (*pools_)[i].ChunkSize_) continue;
            if ((*pools_)[i].head_ == nullptr) continue;
            T* ans = reinterpret_cast<T*>((*pools_)[i].head_);
            (*pools_)[i].head_ = (*pools_)[i].head_->next_;
            return ans;
        }
        throw std::bad_alloc{};
    }

    void deallocate(T* ptr, size_t size){
        for(int i = 0; i < (*pools_).size(); ++i){
            if((*pools_)[i].ChunkSize_ < size) continue;
            T* first = (*pools_)[i].ptr_;
            T* last = first + (*pools_)[i].ChunkSize_ * ((*pools_)[i].PoolSize_ - 1);
            if (ptr < first || ptr > last) continue;
            if ((*pools_)[i].head_ == nullptr){
                (*pools_)[i].head_ = reinterpret_cast<Chunk*>(ptr);
                (*pools_)[i].head_->next_ = nullptr;
                return;
            }
            auto temp = (*pools_)[i].head_;
            (*pools_)[i].head_ = reinterpret_cast<Chunk*>(ptr);
            (*pools_)[i].head_->next_ = temp;
        }
    }
    ~Allocator() = default;
};
