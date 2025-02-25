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

// ��� ����
enum class EIngredientState
{
    OVERCOOKED = -2, // �¿��� ���� ����� �� ����
    NONE = -1,
    COOKED,     // ���� �ϼ�
    CHOPPABLE,  // ��� ��
    GRILLABLE,  // ���� ��
    FRYABLE,    // Ƣ��� ��
    BOILABLE,   // ������ ��
    //...,
    MAX
};

// ���
enum class EIngredientType 
{
    NONE = -1,
    FISH,   // �����
    PRAWN,  // ����
    TOMATO, // �丶��
    ONION,  // ����
    MAX
};

// �丮�� �� ���
struct FCookableIngredient
{
    class AIngredient* Ingredient;     // �ʹ�   , ��
    EIngredientState IngredientState; // �����̽�, ���
};

// �丮�� ������
struct FRecipe
{
    TArray<class AIngredient*> Recipe; // �� + �� + ���� = ���̱��
};

struct FOrder
{
    TArray<class AIngredient*> Dish;
};

// �丮 ��� Ŭ����
// �� ���� ���� ~ ������ �Ǿ� Plate�� �ö󰡱� ������
class AIngredient : public AActor
{
public:
    bool IsCooked() const // ���� �Ϸ�
    {
        return Ingredient.IngredientState== EIngredientState::COOKED;
    }
    bool IsChoppable() const // �����ϴ� ����
    {
        return Ingredient.IngredientState == EIngredientState::CHOPPABLE;
    }

    FString Name;
    FCookableIngredient Ingredient;
    EIngredientType IngredientType;
    float CookingTime = 0.0f;
    float OvercookTime = 0.0f;
};

// ��� �������� �Լ� ����
class Cook
{
public:
    static void Chop(AIngredient* Ingredient)
    {
        // Chopping Logic
        // ���� �ð� ����
        Ingredient->Ingredient.IngredientState = EIngredientState::COOKED; // �����Ϸ�
    }
    static void Grill(AIngredient* Ingredient)
    {
        // Grilling Logic
        Ingredient->Ingredient.IngredientState = EIngredientState::COOKED; // �����Ϸ�
    }
    static void Cooking(AIngredient* Ingredient, std::function<void(AIngredient*)> CookingFunction)
    {
        if (nullptr != Ingredient && nullptr != CookingFunction)
        {
            CookingFunction(Ingredient);
        }
    }
};
// Cook::Cooking(Prawn, Chop);   // ���ϸ� ���.
// Cook::Cooking(Tomato, Grill); // �丶�並 ���´�.


enum class EPlateState
{
    NONE,
    CLEAN,    // ������ ����
    DIRTY,    // ������ ����
    COMPLETE, // �丮�� �ϼ��� ����
    Max,
};

// ���� ~ �ϼ��� �丮���� ��� ������ ����
class APlate : public AActor
{
public:
    void CleanPlate() // ���� ��ô
    {
        // ��ô ����
        Ingredients.Empty();
        PlateState = EPlateState::CLEAN;
    }

    bool Add(AIngredient* Ingredient) // ���ÿ� �丮��Ḧ �߰��Ѵ�.
    {
        // ���� 1. �ݳ� �ı�, �ϼ��� �丮�� ��� ���ô� ����
        if (EPlateState::CLEAN != PlateState) 
        {
            return false;
        }
        // ���� 2. ��ᰡ ������ �ȵ� ���¸� ����
        if (nullptr == Ingredient || Ingredient->Ingredient.IngredientState != EIngredientState::COOKED)
        {
            return false;
        }
        // if ������ �Ұ��� ���� ������ ���� ��� �ƴ°�?

        Recipe.Recipe.Add(Ingredient);
        IsCompleteDish();

        return true;
    }

    bool IsCompleteDish() // �丮�� �ϼ��Ǿ�����
    {
        Recipe.Recipe.Sort();
        TArray<FOrder>& OrderList = Global::GetOrderList();
        for (const FOrder& Order : OrderList)
        {
            Order.Dish.Sort();
            if (Order == Recipe.Recipe)
            {
                // �ϼ��� �丮�� �޽� �ٲٰ�
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

// �������� �� �����ϰ� ��� �ٴ� �� �ִ� �༮�ε�. 
// ����������� ���� Ŭ������ ���� Table�� ������ ��� �и��� ���ΰ�?
// �ƴϸ� Pot�� �и��� ���ΰ�?
class APot : public AActor
{

};

// ���̺��� ��� �� ���ΰ�?
// ���̺��� ��ӹ޴� ���� ���̺�, ���������� ���̺�? �׳� ���̺� �ϳ���?
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

