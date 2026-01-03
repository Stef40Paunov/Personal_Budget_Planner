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
    STR_TITLE = 0,
    STR_ADD_TRANSACTION,
    STR_VIEW_TRANSACTIONS,
    STR_VIEW_SUMMARY,
    STR_SETTINGS,
    STR_SAVE_EXIT,
    STR_ENTER_CHOICE,
    STR_INVALID_CHOICE,
    STR_TRANSACTION_ADDED,
    STR_TYPE_PROMPT,
    STR_AMOUNT_PROMPT,
    STR_CURRENCY_PROMPT,
    STR_CATEGORY_PROMPT,
    STR_CATEGORY_OPTIONS,
    STR_DESCRIPTION_PROMPT,
    STR_ALL_TRANSACTIONS,
    STR_NO_TRANSACTIONS,
    STR_TOTAL_INCOME,
    STR_TOTAL_EXPENSES,
    STR_BALANCE,
    STR_SETTINGS_MENU,
    STR_CHANGE_LANGUAGE,
    STR_CHANGE_CURRENCY,
    STR_BACK_TO_MENU,
    STR_CURRENT_LANGUAGE,
    STR_ENGLISH,
    STR_BULGARIAN,
    STR_SELECT_LANGUAGE,
    STR_CURRENT_CURRENCY,
    STR_SELECT_CURRENCY,
    STR_DATA_SAVED,
    STR_PRESS_ENTER
} StringID;

// Transaction structure
typedef struct {
    int id;
    char date[11]; // YYYY-MM-DD
    float amount;
    Currency currency;
    TransactionType type;
    Category category;
    char description[MAX_STRING];
} Transaction;

// User settings structure
typedef struct {
    Language language;
    Currency default_currency;
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
    {"2. Change Default Currency", "2. Смени Валута по Подразбиране"},
    {"3. Back to Main Menu", "3. Обратно към Главното Меню"},
    {"Current Language: ", "Текущ Език: "},
    {"English", "Английски"},
    {"Bulgarian", "Български"},
    {"Select language (0=English, 1=Bulgarian): ", "Избери език (0=Английски, 1=Български): "},
    {"Current Default Currency: ", "Текуща Валута: "},
    {"Select currency (0=EUR, 1=BGN): ", "Избери валута (0=EUR, 1=BGN): "},
    {"Data saved successfully!", "Данните са запазени успешно!"},
    {"Press Enter to continue...", "Натисни Enter за да продължиш..."}
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

int main(void) {
    initialize_settings();
    load_data();
    
    int choice;
    
    while (1) {
        clear_screen();
        print_menu();
        printf("%s", get_string(6));
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); // Clear input buffer
            printf("%s\n", get_string(7));
            printf("%s", get_string(33));
            getchar();
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
                printf("%s\n", get_string(32));
                return 0;
            default:
                printf("%s\n", get_string(7));
        }
        
        printf("\n%s", get_string(33));
        getchar();
    }
    
    return 0;
}

void initialize_settings() {
    settings.language = ENGLISH;
    settings.default_currency = BGN;
    
    // Try to load settings from config file
    FILE* file = fopen(CONFIG_FILE, "r");
    if (file != NULL) {
        fscanf(file, "%d %d", (int*)&settings.language, (int*)&settings.default_currency);
        fclose(file);
    }
}

void save_settings() {
    FILE* file = fopen(CONFIG_FILE, "w");
    if (file != NULL) {
        fprintf(file, "%d %d\n", settings.language, settings.default_currency);
        fclose(file);
    }
}

void load_data() {
    FILE* file = fopen(DATA_FILE, "r");
    if (file == NULL) {
        return; // No data file yet
    }
    
    transaction_count = 0;
    while (fscanf(file, "%d %s %f %d %d %d %[^\n]", 
                  &transactions[transaction_count].id,
                  transactions[transaction_count].date,
                  &transactions[transaction_count].amount,
                  (int*)&transactions[transaction_count].currency,
                  (int*)&transactions[transaction_count].type,
                  (int*)&transactions[transaction_count].category,
                  transactions[transaction_count].description) == 7) {
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
        fprintf(file, "%d %s %.2f %d %d %d %s\n",
                transactions[i].id,
                transactions[i].date,
                transactions[i].amount,
                transactions[i].currency,
                transactions[i].type,
                transactions[i].category,
                transactions[i].description);
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
    printf("╔════════════════════════════════════════╗\n");
    printf("║  %s  ║\n", get_string(STR_TITLE));
    printf("╚════════════════════════════════════════╝\n\n");
    printf("%s\n", get_string(STR_ADD_TRANSACTION));
    printf("%s\n", get_string(STR_VIEW_TRANSACTIONS));
    printf("%s\n", get_string(STR_VIEW_SUMMARY));
    printf("%s\n", get_string(STR_SETTINGS));
    printf("%s\n\n", get_string(STR_SAVE_EXIT));
}

void get_current_date(char* buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

void add_transaction() {
    clear_screen();
    
    if (transaction_count >= MAX_TRANSACTIONS) {
        printf("Transaction limit reached!\n");
        return;
    }
    
    Transaction new_trans;
    new_trans.id = transaction_count + 1;
    get_current_date(new_trans.date);
    
    int type_input;
    printf("%s", get_string(STR_TYPE_PROMPT));
    scanf("%d", &type_input);
    new_trans.type = (TransactionType)type_input;
    
    printf("%s", get_string(STR_AMOUNT_PROMPT));
    scanf("%f", &new_trans.amount);
    
    int curr_input;
    printf("%s", get_string(STR_CURRENCY_PROMPT));
    scanf("%d", &curr_input);
    new_trans.currency = (Currency)curr_input;
    
    printf("\n%s\n", get_string(STR_CATEGORY_PROMPT));
    printf("%s\n", get_string(STR_CATEGORY_OPTIONS));
    int cat_input;
    scanf("%d", &cat_input);
    new_trans.category = (Category)cat_input;
    
    while(getchar() != '\n'); // Clear buffer
    printf("%s", get_string(STR_DESCRIPTION_PROMPT));
    fgets(new_trans.description, MAX_STRING, stdin);
    new_trans.description[strcspn(new_trans.description, "\n")] = 0; // Remove newline
    
    transactions[transaction_count++] = new_trans;
    
    printf("\n%s\n", get_string(STR_TRANSACTION_ADDED));
}

void view_transactions() {
    clear_screen();
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("%s\n", get_string(STR_ALL_TRANSACTIONS));
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    if (transaction_count == 0) {
        printf("%s\n", get_string(STR_NO_TRANSACTIONS));
        return;
    }
    
    for (int i = 0; i < transaction_count; i++) {
        Transaction t = transactions[i];
        printf("[%d] %s | %.2f %s | %s | %s\n",
               t.id,
               t.date,
               t.amount,
               currency_symbols[t.currency],
               t.type == INCOME ? "+" : "-",
               category_names[t.category][settings.language]);
        printf("    %s\n\n", t.description);
    }
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
    clear_screen();
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("%s\n", get_string(STR_VIEW_SUMMARY));
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
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
    
    // Display in both currencies
    printf("%s %.2f EUR (%.2f BGN)\n", get_string(STR_TOTAL_INCOME), 
           total_income_eur, 
           convert_currency(total_income_eur, EUR, BGN));
    
    printf("%s %.2f EUR (%.2f BGN)\n", get_string(STR_TOTAL_EXPENSES), 
           total_expense_eur, 
           convert_currency(total_expense_eur, EUR, BGN));
    
    printf("\n%s %.2f EUR (%.2f BGN)\n", get_string(STR_BALANCE), 
           balance, 
           convert_currency(balance, EUR, BGN));
    
    // Show percentage breakdown by category
    printf("\n--- Expenses by Category ---\n");
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

void settings_menu() {
    int choice;
    
    while (1) {
        clear_screen();
        printf("═══════════════════════════════════════\n");
        printf("%s\n", get_string(STR_SETTINGS_MENU));
        printf("═══════════════════════════════════════\n\n");
        printf("%s\n", get_string(STR_CHANGE_LANGUAGE));
        printf("%s\n", get_string(STR_CHANGE_CURRENCY));
        printf("%s\n\n", get_string(STR_BACK_TO_MENU));
        
        printf("%s%s\n", get_string(STR_CURRENT_LANGUAGE), 
               get_string(STR_ENGLISH + settings.language));
        printf("%s%s\n", get_string(STR_CURRENT_CURRENCY), 
               currency_symbols[settings.default_currency]);
        printf("Exchange Rate: 1 EUR = %.5f BGN (Fixed)\n\n", EXCHANGE_RATE);
        
        printf("%s", get_string(STR_ENTER_CHOICE));
        scanf("%d", &choice);
        while(getchar() != '\n');
        
        switch(choice) {
            case 1: {
                int lang;
                printf("%s", get_string(STR_SELECT_LANGUAGE));
                scanf("%d", &lang);
                while(getchar() != '\n');
                if (lang == 0 || lang == 1) {
                    settings.language = (Language)lang;
                }
                break;
            }
            case 2: {
                int curr;
                printf("%s", get_string(STR_SELECT_CURRENCY));
                scanf("%d", &curr);
                while(getchar() != '\n');
                if (curr == 0 || curr == 1) {
                    settings.default_currency = (Currency)curr;
                }
                break;
            }
            case 3:
                save_settings();
                return;
            default:
                printf("%s\n", get_string(STR_INVALID_CHOICE));
        }
    }
}