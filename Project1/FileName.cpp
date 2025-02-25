#include <functional>

class AActor
{
};
class FString
{
};

template<typename T>
class TArray
{
public:
    void Add(T _T) {}
    void Empty() {}
    void Sort() {}
};

class Global
{
public:
    static class TArray<FOrder>& GetOrderList();
};

// 재료 상태
enum class EIngredientState
{
    OVERCOOKED = -2, // 태워서 재료로 사용할 수 없음
    NONE = -1,
    COOKED,     // 조리 완성
    CHOPPABLE,  // 썰기 전
    GRILLABLE,  // 굽기 전
    FRYABLE,    // 튀기기 전
    BOILABLE,   // 밥짓기 전
    //...,
    MAX
};

// 재료
enum class EIngredientType 
{
    NONE = -1,
    FISH,   // 물고기
    PRAWN,  // 대하
    TOMATO, // 토마토
    ONION,  // 양파
    MAX
};

// 요리에 들어갈 재료
struct FCookableIngredient
{
    class AIngredient* Ingredient;     // 초밥   , 밥
    EIngredientState IngredientState; // 슬라이스, 취사
};

// 요리의 레시피
struct FRecipe
{
    TArray<class AIngredient*> Recipe; // 밥 + 김 + 오이 = 오이김밥
};

struct FOrder
{
    TArray<class AIngredient*> Dish;
};

// 요리 재료 클래스
// 날 것의 상태 ~ 조리가 되어 Plate에 올라가기 전까지
class AIngredient : public AActor
{
public:
    bool IsCooked() const // 조리 완료
    {
        return Ingredient.IngredientState== EIngredientState::COOKED;
    }
    bool IsChoppable() const // 손질하는 재료냐
    {
        return Ingredient.IngredientState == EIngredientState::CHOPPABLE;
    }

    FString Name;
    FCookableIngredient Ingredient;
    EIngredientType IngredientType;
    float CookingTime = 0.0f;
    float OvercookTime = 0.0f;
};

// 재료 손질관련 함수 집합
class Cook
{
public:
    static void Chop(AIngredient* Ingredient)
    {
        // Chopping Logic
        // 일정 시간 이후
        Ingredient->Ingredient.IngredientState = EIngredientState::COOKED; // 조리완료
    }
    static void Grill(AIngredient* Ingredient)
    {
        // Grilling Logic
        Ingredient->Ingredient.IngredientState = EIngredientState::COOKED; // 조리완료
    }
    static void Cooking(AIngredient* Ingredient, std::function<void(AIngredient*)> CookingFunction)
    {
        if (nullptr != Ingredient && nullptr != CookingFunction)
        {
            CookingFunction(Ingredient);
        }
    }
};
// Cook::Cooking(Prawn, Chop);   // 대하를 썬다.
// Cook::Cooking(Tomato, Grill); // 토마토를 굽는다.


enum class EPlateState
{
    NONE,
    CLEAN,    // 깨끗한 접시
    DIRTY,    // 더러운 접시
    COMPLETE, // 요리가 완성된 접시
    Max,
};

// 접시 ~ 완성된 요리까지 모든 개념을 통합
class APlate : public AActor
{
public:
    void CleanPlate() // 접시 세척
    {
        // 세척 로직
        Ingredients.Empty();
        PlateState = EPlateState::CLEAN;
    }

    bool Add(AIngredient* Ingredient) // 접시에 요리재료를 추가한다.
    {
        // 조건 1. 반납 식기, 완성된 요리가 담긴 접시는 리턴
        if (EPlateState::CLEAN != PlateState) 
        {
            return false;
        }
        // 조건 2. 재료가 손질이 안된 상태면 리턴
        if (nullptr == Ingredient || Ingredient->Ingredient.IngredientState != EIngredientState::COOKED)
        {
            return false;
        }
        // if 조합이 불가한 재료들 끼리인 것을 어떻게 아는가?

        Recipe.Recipe.Add(Ingredient);
        IsCompleteDish();

        return true;
    }

    bool IsCompleteDish() // 요리가 완성되었는지
    {
        Recipe.Recipe.Sort();
        TArray<FOrder>& OrderList = Global::GetOrderList();
        for (const FOrder& Order : OrderList)
        {
            Order.Dish.Sort();
            if (Order == Recipe.Recipe)
            {
                // 완성된 요리로 메시 바꾸고
                return true;
            }
        }

        return false;
    }

private:
    EPlateState PlateState = EPlateState::NONE;
    TArray<AIngredient*> Ingredients;
    FRecipe Recipe;
};

// 조리도구 중 유일하게 들고 다닐 수 있는 녀석인데. 
// 조리도구라는 상위 클래스를 만들어서 Table과 도구는 모두 분리할 것인가?
// 아니면 Pot만 분리할 것인가?
class APot : public AActor
{

};

// 테이블은 어떻게 할 것인가?
// 테이블을 상속받는 도마 테이블, 가스레인지 테이블? 그냥 테이블 하나로?
class ATable : public AActor
{
public:
    void PlacePlate(APlate* Plate)
    {
        PlacedPlate = Plate;
    }
    bool PlaceIngredient(AIngredient* Ingredient)
    {
        if (false == PlacedPlate->AddIngredient(Ingredient))
        {
            return;
        }      
        Ingredients.Add(Ingredient);
    }

private:
    TArray<AIngredient*> Ingredients;
    APlate* PlacedPlate;
};

