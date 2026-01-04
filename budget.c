#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TRANSACTIONS 1000
#define MAX_STRING 100
#define DATA_FILE "budget_data.txt"
#define CONFIG_FILE "config.txt"
#define EXCHANGE_RATE 1.95583 // Fixed EUR to BGN rate

// Language enum
typedef enum {
    ENGLISH = 0,
    BULGARIAN = 1
} Language;

// Currency enum
typedef enum {
    EUR = 0,
    BGN = 1
} Currency;

// Transaction type
typedef enum {
    INCOME = 0,
    EXPENSE = 1
} TransactionType;

// Category enum
typedef enum {
    FOOD = 0,
    TRANSPORT,
    ENTERTAINMENT,
    BILLS,
    SALARY,
    OTHER
} Category;

// String IDs for multi-language support
typedef enum {
    TITLE = 0,
    ADD_TRANSACTION,
    VIEW_TRANSACTIONS,
    VIEW_SUMMARY,
    SETTINGS,
    SAVE_EXIT,
    ENTER_CHOICE,
    INVALID_CHOICE,
    TRANSACTION_ADDED,
    TYPE_PROMPT,
    AMOUNT_PROMPT,
    CURRENCY_PROMPT,
    CATEGORY_PROMPT,
    CATEGORY_OPTIONS,
    DESCRIPTION_PROMPT,
    ALL_TRANSACTIONS,
    NO_TRANSACTIONS,
    TOTAL_INCOME,
    TOTAL_EXPENSES,
    BALANCE,
    SETTINGS_MENU,
    CHANGE_LANGUAGE,
    BACK_TO_MENU,
    CURRENT_LANGUAGE,
    LANG_ENGLISH,
    LANG_BULGARIAN,
    SELECT_LANGUAGE,
    DATA_SAVED,
    PRESS_ENTER,
    INVALID_INPUT
} StringID;

// Transaction structure
typedef struct {
    int id;
    char date[11]; // YYYY-MM-DD
    float amount;
    Currency currency;
    TransactionType type;
    Category category;
} Transaction;

// User settings structure
typedef struct {
    Language language;
} UserSettings;

// Global variables
Transaction transactions[MAX_TRANSACTIONS];
int transaction_count = 0;
UserSettings settings;

// Multi-language strings
const char* menu_strings[][2] = {
    {"Personal Budget Planner", "Личен Бюджетен Планер"},
    {"1. Add Transaction", "1. Добави Транзакция"},
    {"2. View All Transactions", "2. Виж Всички Транзакции"},
    {"3. View Summary", "3. Виж Обобщение"},
    {"4. Settings", "4. Настройки"},
    {"5. Save and Exit", "5. Запази и Изход"},
    {"Enter choice: ", "Избери опция: "},
    {"Invalid choice!", "Невалиден избор!"},
    {"Transaction added successfully!", "Транзакцията е добавена успешно!"},
    {"Type (0=Income, 1=Expense): ", "Вид (0=Приход, 1=Разход): "},
    {"Amount: ", "Сума: "},
    {"Currency (0=EUR, 1=BGN): ", "Валута (0=EUR, 1=BGN): "},
    {"Category:", "Категория:"},
    {"0=Food, 1=Transport, 2=Entertainment, 3=Bills, 4=Salary, 5=Other", "0=Храна, 1=Транспорт, 2=Развлечения, 3=Сметки, 4=Заплата, 5=Друго"},
    {"Description: ", "Описание: "},
    {"All Transactions:", "Всички Транзакции:"},
    {"No transactions yet.", "Няма транзакции все още."},
    {"Total Income:", "Общо Приходи:"},
    {"Total Expenses:", "Общо Разходи:"},
    {"Balance:", "Баланс:"},
    {"Settings Menu", "Меню Настройки"},
    {"1. Change Language", "1. Смени Език"},
    {"2. Back to Main Menu", "2. Обратно към Главното Меню"},
    {"Current Language: ", "Текущ Език: "},
    {"English", "Английски"},
    {"Bulgarian", "Български"},
    {"Select language (0=English, 1=Bulgarian): ", "Избери език (0=Английски, 1=Български): "},
    {"Data saved successfully!", "Данните са запазени успешно!"},
    {"\nPress Enter to continue...", "\nНатисни Enter за да продължиш..."},
    {"Invalid input!", "Невалидни данни!"}
};

// Category names
const char* category_names[][2] = {
    {"Food", "Храна"},
    {"Transport", "Транспорт"},
    {"Entertainment", "Развлечения"},
    {"Bills", "Сметки"},
    {"Salary", "Заплата"},
    {"Other", "Друго"}
};

// Currency symbols
const char* currency_symbols[] = {"EUR", "BGN"};

// Function prototypes
void initialize_settings();
void load_data();
void save_data();
void clear_screen();
void print_menu();
void add_transaction();
void view_transactions();
void view_summary();
void settings_menu();
const char* get_string(StringID id);
void get_current_date(char* buffer);
float convert_currency(float amount, Currency from, Currency to);
void wait_for_enter(void);

int main(void) {
    initialize_settings();
    load_data();
    
    int choice;
    
    while (1) {
        clear_screen();
        print_menu();
        printf("%s", get_string(ENTER_CHOICE));
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); // Clear input buffer
            printf("%s\n", get_string(INVALID_CHOICE));
            printf("%s", get_string(PRESS_ENTER));
            wait_for_enter();
            continue;
        }
        while(getchar() != '\n'); // Clear newline
        
        switch(choice) {
            case 1:
                add_transaction();
                break;
            case 2:
                view_transactions();
                break;
            case 3:
                view_summary();
                break;
            case 4:
                settings_menu();
                break;
            case 5:
                save_data();
                return 0;
            default:
                printf("%s\n", get_string(INVALID_CHOICE));
                printf("%s", get_string(PRESS_ENTER));
                break;
        }
        wait_for_enter();        
    }
    
    return 0;
}

void initialize_settings() {
    settings.language = ENGLISH;
    
    // Try to load settings from config file
    FILE* file = fopen(CONFIG_FILE, "r");
    if (file != NULL) {
        fscanf(file, "%d", (int*)&settings.language);
        fclose(file);
    }
}

void save_settings() {
    FILE* file = fopen(CONFIG_FILE, "w");
    if (file != NULL) {
        fprintf(file, "%d\n", settings.language);
        fclose(file);
    }
}

void load_data() {
    FILE* file = fopen(DATA_FILE, "r");
    if (file == NULL) {
        return; // No data file yet
    }
    
    transaction_count = 0;
    while (fscanf(file, "%d %s %f %d %d %d", 
                  &transactions[transaction_count].id,
                  transactions[transaction_count].date,
                  &transactions[transaction_count].amount,
                  (int*)&transactions[transaction_count].currency,
                  (int*)&transactions[transaction_count].type,
                  (int*)&transactions[transaction_count].category) == 6) {
        transaction_count++;
        if (transaction_count >= MAX_TRANSACTIONS) break;
    }
    
    fclose(file);
}

void save_data() {
    FILE* file = fopen(DATA_FILE, "w");
    if (file == NULL) {
        printf("Error saving data!\n");
        return;
    }
    
    for (int i = 0; i < transaction_count; i++) {
        fprintf(file, "%d %s %.2f %d %d %d\n",
                transactions[i].id,
                transactions[i].date,
                transactions[i].amount,
                transactions[i].currency,
                transactions[i].type,
                transactions[i].category);
    }
    
    fclose(file);
    save_settings();
}

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

const char* get_string(StringID id) {
    return menu_strings[id][settings.language];
}

void print_menu() {
    printf("\n══════════════════════════════════════════════════\n");
    printf("  %s\n", get_string(TITLE));
    printf("══════════════════════════════════════════════════\n\n");
    printf("%s\n", get_string(ADD_TRANSACTION));
    printf("%s\n", get_string(VIEW_TRANSACTIONS));
    printf("%s\n", get_string(VIEW_SUMMARY));
    printf("%s\n", get_string(SETTINGS));
    printf("%s\n\n", get_string(SAVE_EXIT));
}

void get_current_date(char* buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

void add_transaction() {
    if (transaction_count >= MAX_TRANSACTIONS) {
        printf("%s\n", get_string(INVALID_INPUT));
        printf("%s\n", get_string(PRESS_ENTER));

        return;
    }
    
    Transaction new_trans;
    new_trans.id = transaction_count + 1;
    get_current_date(new_trans.date);
    
    printf("\n%s", get_string(TYPE_PROMPT));
    int type_input;
    while (1){
    if (scanf("%d", &type_input) != 1 || (type_input != 0 && type_input != 1))
    {
        while (getchar() != '\n');
        printf("%s\n", get_string(INVALID_INPUT));
        printf("%s", get_string(TYPE_PROMPT));
        continue;
    }

        break;

}
    new_trans.type = (TransactionType)type_input;
    
    printf("%s", get_string(AMOUNT_PROMPT));

    while (1){
    if (scanf("%f", &new_trans.amount) != 1 || new_trans.amount <= 0)
    {
        while (getchar() != '\n');
        printf("%s\n", get_string(INVALID_INPUT));
        printf("%s", get_string(AMOUNT_PROMPT));
        continue;
    }
    break;
}
    
    printf("%s", get_string(CURRENCY_PROMPT));

int curr_input;

while (1){
    if (scanf("%d", &curr_input) != 1 ||
        (curr_input != 0 && curr_input != 1))
    {
        while (getchar() != '\n');
        printf("%s\n", get_string(INVALID_INPUT));
        printf("%s", get_string(CURRENCY_PROMPT));
        continue;
    }
     break;
}
new_trans.currency = (Currency) curr_input;
    
printf("\n%s\n", get_string(CATEGORY_PROMPT));
printf("%s\n", get_string(CATEGORY_OPTIONS));
int cat_input;

while (1){
    if (scanf("%d", &cat_input) != 1 ||
        cat_input < 0 || cat_input > 5)
    {
        while (getchar() != '\n');
        printf("%s\n", get_string(INVALID_INPUT));
        printf("\n%s\n", get_string(CATEGORY_PROMPT));
        printf("%s\n", get_string(CATEGORY_OPTIONS));
        continue;
    }
    break;
}
new_trans.category = (Category) cat_input;
    
while(getchar() != '\n');

transactions[transaction_count++] = new_trans;

printf("\n%s\n", get_string(TRANSACTION_ADDED));
printf("%s\n", get_string(PRESS_ENTER));
}

void view_transactions() {
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("%s\n", get_string(ALL_TRANSACTIONS));
    printf("═══════════════════════════════════════════════════════════════\n");
    
    if (transaction_count == 0) {
        printf("%s\n", get_string(NO_TRANSACTIONS));
        printf("%s\n", get_string(PRESS_ENTER));
        return;
    }
    
    for (int i = 0; i < transaction_count; i++) {
        Transaction t = transactions[i];
        printf("\n[%d] %s | %.2f %s | %s | %s",
               t.id,
               t.date,
               t.amount,
               currency_symbols[t.currency],
               t.type == INCOME ? "+" : "-",
               category_names[t.category][settings.language]);
    }
    printf("\n");
    printf("%s\n", get_string(PRESS_ENTER));
}

float convert_currency(float amount, Currency from, Currency to) {
    if (from == to) return amount;
    
    if (from == EUR && to == BGN) {
        return amount * EXCHANGE_RATE;
    } else { // from == BGN && to == EUR
        return amount / EXCHANGE_RATE;
    }
}

void view_summary() {
    printf("\n═══════════════════════════════════════════════════════════════\n");
    const char* summary_title = (settings.language == ENGLISH) ? "Summary" : "Обобщение";
    printf("%s\n", summary_title);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    float total_income_eur = 0, total_expense_eur = 0;
    
    for (int i = 0; i < transaction_count; i++) {
        float amount_eur = convert_currency(transactions[i].amount, 
                                           transactions[i].currency, 
                                           EUR);
        
        if (transactions[i].type == INCOME) {
            total_income_eur += amount_eur;
        } else {
            total_expense_eur += amount_eur;
        }
    }
    
    float balance = total_income_eur - total_expense_eur;
    
    printf("\n%s %.2f EUR (%.2f BGN)\n", get_string(TOTAL_INCOME), 
           total_income_eur, 
           convert_currency(total_income_eur, EUR, BGN));
    
    printf("%s %.2f EUR (%.2f BGN)\n", get_string(TOTAL_EXPENSES), 
           total_expense_eur, 
           convert_currency(total_expense_eur, EUR, BGN));
    
    printf("\n%s %.2f EUR (%.2f BGN)\n", get_string(BALANCE), 
           balance, 
           convert_currency(balance, EUR, BGN));
    
    if (total_expense_eur > 0) {
        const char* expenses_title = (settings.language == ENGLISH) ? "\n--- Expenses by Category ---\n" : "\n--- Разходи по категория ---\n";
        printf("%s\n", expenses_title);
        for (int cat = 0; cat < 6; cat++) {
            float cat_total = 0;
            for (int i = 0; i < transaction_count; i++) {
                if (transactions[i].type == EXPENSE && transactions[i].category == cat) {
                    cat_total += convert_currency(transactions[i].amount, 
                                                 transactions[i].currency, 
                                                 EUR);
                }
            }
            if (cat_total > 0) {
                float percentage = (cat_total / total_expense_eur) * 100;
                printf("%s: %.2f EUR (%.1f%%)\n", 
                       category_names[cat][settings.language], 
                       cat_total, 
                       percentage);
            }
        }
    }
    printf("%s\n", get_string(PRESS_ENTER));
}

void settings_menu() {
    int choice;
    
    while (1) {
        clear_screen();
        printf("═══════════════════════════════════════\n");
        printf("%s\n", get_string(SETTINGS_MENU));
        printf("═══════════════════════════════════════\n\n");
        printf("%s\n", get_string(CHANGE_LANGUAGE));
        printf("%s\n\n", get_string(BACK_TO_MENU));
        
        printf("%s%s\n", get_string(CURRENT_LANGUAGE), 
               get_string(LANG_ENGLISH + settings.language));
        printf("Exchange Rate: 1 EUR = %.5f BGN (Fixed)\n\n", EXCHANGE_RATE);
        
        printf("%s", get_string(ENTER_CHOICE));
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            printf("%s\n", get_string(INVALID_CHOICE));
            printf("%s", get_string(PRESS_ENTER));
            wait_for_enter();
            continue;
        }
        while(getchar() != '\n');
        
        switch(choice) {
            case 1: {
                int lang;
                printf("%s", get_string(SELECT_LANGUAGE));
                if (scanf("%d", &lang) != 1 || (lang != 0 && lang != 1)) {
                    while(getchar() != '\n');
                    printf("%s\n", get_string(INVALID_INPUT));
                    printf("%s", get_string(PRESS_ENTER));
                    wait_for_enter();
                } else {
                    while(getchar() != '\n');
                    settings.language = (Language)lang;
                }
                break;
            }
            case 2:
                save_settings();
                printf("%s", get_string(PRESS_ENTER));
                return;
            default:
                printf("%s\n", get_string(INVALID_CHOICE));
                printf("%s", get_string(PRESS_ENTER));
                wait_for_enter();
        }
    }
}
void wait_for_enter() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}