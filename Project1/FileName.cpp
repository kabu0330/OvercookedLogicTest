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
    bool operator==(const TArray<T>& _T) {}
};

class Global
{
public:
    static class TArray<FOrder>& GetOrderList() {}
};

// ��� ����
enum class EIngredientState
{
    OVERCOOKED = -2, // �¿��� ���� ����� �� ����
    NONE = -1,
    COOKED,     // ���� �ϼ� => Plate�� �ö� �� �ִ� ������ ����
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
    EIngredientType IngredientType;       //  ����� , ��
    EIngredientState IngredientState; // �����̽�, ���
};

// �丮�� ������
struct FRecipe
{
    TArray<EIngredientType> Recipe; // �� + �� + ���� = ���̱��
};

struct FOrder
{
    TArray<EIngredientType> Dish; // �� + �� + ���� = ���̱�� 

    bool operator==(FRecipe Recipe) 
    {
        // Sort�ϰ� ��
        Dish.Sort();
        Recipe.Recipe.Sort();
        if (Dish == Recipe.Recipe)
        {
            return true;
        }
        return false;
    }

};

// �丮 ��� Ŭ����
// �� ���� ���� ~ ������ �Ǿ� Plate�� �ö󰡱� ������
class AIngredient : public AActor
{
public:
    bool IsCooked() const // ���� �Ϸ�
    {
        return IngredientData.IngredientState== EIngredientState::COOKED;
    }
    bool IsChoppable() const // �����ϴ� ����
    {
        return IngredientData.IngredientState == EIngredientState::CHOPPABLE;
    }

    FString Name;
    FCookableIngredient IngredientData;
    float CookingTime = 0.0f;
    float OvercookTime = 0.0f;
};

// ��� �������� �Լ� ����
class Cook
{
public:
    static void Chop(AIngredient* Ingredient)
    {
        // Chopping Logic <= ��� Time�� �� ���ΰ�? �� �Լ��� 1ȸ�� ȣ�� �Լ���, Actor ����� �ƴѵ�... �� �Լ� ������?
        // �ٸ� ������ �����ؾ��ϳ�?
        
        // ���� �ð� ����
        Ingredient->IngredientData.IngredientState = EIngredientState::COOKED; // �����Ϸ�
    }
    static void Grill(AIngredient* Ingredient)
    {
        // Grilling Logic
        Ingredient->IngredientData.IngredientState = EIngredientState::COOKED; // �����Ϸ�
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
        if (nullptr == Ingredient || Ingredient->IngredientData.IngredientState != EIngredientState::COOKED)
        {
            return false;
        }
        // if ������ �Ұ��� ���� ������ ���� ��� �ƴ°�?

        Recipe.Recipe.Add(Ingredient->IngredientData.IngredientType);
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

    // ��ᰡ �߰��� �� ���� �޽ø� �ٲ��ش�.
    void SetMeshArrayIndex(int MeshIndex) {}

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
        // ���̺� ���ð� ����� �ٸ� ���ø� �ø� �� �ִ�.
        if (nullptr == PlacedPlate) 
        {
            PlacedPlate = Plate;
        }
    }
    bool PlaceIngredient(AIngredient* Ingredient)
    {
        // ���̺��� ��Ḧ �ø� ��, Plate�� ó����.
        if (true == PlacedPlate->Add(Ingredient))
        {
            // ���� ���� ���� ������ �� �˰� ��� �޽ø� �ٲ���?
            PlacedPlate->SetMeshArrayIndex(1);
            return true;
        }      
        return false;
    }

private:
    APlate* PlacedPlate;
};

