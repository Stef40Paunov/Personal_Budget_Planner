#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TRANSACTIONS 1000
#define MAX_RECURRING 100
#define MAX_STRING 100
#define DATA_FILE "budget_data.txt"
#define RECURRING_FILE "recurring_data.txt"
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

// Recurrence type
typedef enum {
    DAILY = 0,
    WEEKLY = 1,
    MONTHLY = 2,
    YEARLY = 3
} RecurrenceType;

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
    SET_BUDGET_LIMITS,
    BACK_TO_MENU,
    CURRENT_LANGUAGE,
    LANG_ENGLISH,
    LANG_BULGARIAN,
    SELECT_LANGUAGE,
    DATA_SAVED,
    PRESS_ENTER,
    INVALID_INPUT,
    FILTER_CHOICE,
    SHOW_ALL_OPTION,
    FILTER_OPTION,
    ENTER_FROM_YEAR,
    ENTER_FROM_MONTH,
    ENTER_FROM_DAY,
    ENTER_TO_YEAR,
    ENTER_TO_MONTH,
    ENTER_TO_DAY,
    END_DATE_BEFORE_START,
    ID_TEXT,
    DATE_TEXT,
    AMOUNT_TEXT,
    TYPE_TEXT,
    CATEGORY_TEXT,
    INVALID_DATE, 
    SET_LIMITS_PROMPT, 
    TOTAL_LIMIT_PROMPT,
    TOTAL_LIMIT_EXCEEDED,
    CATEGORY_LIMIT_EXCEEDED,
    EXCHANGE_RATE_TEXT,
    CURRENT_BUDGET_LIMITS,
    CURRENT_TOTAL_LIMIT,
    CURRENT_CATEGORY_LIMIT,
    IS_RECURRING_PROMPT,
    RECURRENCE_TYPE_PROMPT,
    RECURRENCE_OPTIONS,
    RECURRING_ADDED,
    RECURRING_PROCESSED
} StringID;

typedef enum {
    SHOW_ALL = 0,
    FILTER_BY_DATE = 1
} ViewMode;

// Transaction structure
typedef struct {
    int id;
    char date[11]; // YYYY-MM-DD
    float amount;
    Currency currency;
    TransactionType type;
    Category category;
} Transaction;

// Recurring transaction structure
typedef struct {
    int id;
    char start_date[11];
    char last_processed[11];
    float amount;
    Currency currency;
    TransactionType type;
    Category category;
    RecurrenceType recurrence;
    int active; // 1 = active, 0 = inactive
} RecurringTransaction;

// User settings structure
typedef struct {
    Language language;
} UserSettings;

typedef struct {
    int from_year, from_month, from_day;
    int to_year, to_month, to_day;
} DateRange;

typedef struct {
    float category_limits[OTHER + 1];
    float total_limit;
} BudgetLimits;

// Global variables
Transaction transactions[MAX_TRANSACTIONS];
int transaction_count = 0;
RecurringTransaction recurring_transactions[MAX_RECURRING];
int recurring_count = 0;
UserSettings settings;
BudgetLimits budget_limits;

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
    {"2. Set Budget Limits", "2. Настрой Лимити"},
    {"3. Back to Main Menu", "3. Обратно към Главното Меню"},
    {"Current Language: ", "Текущ Език: "},
    {"English", "Английски"},
    {"Bulgarian", "Български"},
    {"Select language (0=English, 1=Bulgarian): ", "Избери език (0=Английски, 1=Български): "},
    {"Data saved successfully!", "Данните са запазени успешно!"},
    {"\nPress Enter to continue...", "\nНатисни Enter за да продължиш..."},
    {"Invalid input!", "Невалидни данни!"},
    {"View mode:", "Режим на преглед:"},
    {"0. Show all", "0. Виж всички"},
    {"1. Filter by date", "1. Филтриране по дата"},
    {"Start year: ", "Начална година: "},
    {"Start month (1-12): ", "Начален месец (1-12): "},
    {"Start day (1-31): ", "Начален ден (1-31): "},
    {"End year: ", "Крайна година: "},
    {"End month (1-12): ", "Краен месец (1-12): "},
    {"End day (1-31): ", "Краен ден (1-31): "},
    {"End date is before start date!", "Крайната дата е преди началната!"},
    {"ID", "ИД"},
    {"Date", "Дата"},
    {"Amount", "Сума"},
    {"+/-", "+/-"},
    {"Category", "Категория"},
    {"Invalid date!", "Невалидна дата!"},
    {"Set Budget Limits", "Настрой Лимити"},
    {"Enter total budget limit (0 = no limit): ", "Общ лимит (0 = няма лимит): "},
    {"Total budget limit exceeded!", "Общият лимит е надвишен!"},
    {"Category budget limit exceeded!", "Лимитът за тази категория е надвишен!"},
    {"Exchange Rate (1 EUR = 1.95583 BGN)", "Обменен курс (1 EUR = 1.95583 BGN)"},
    {"Current Budget Limits", "Текущи Бюджетни Лимити"},
    {"Total limit: %.2f EUR\n", "Общ лимит: %.2f EUR\n"},
    {"%s limit: %.2f EUR\n", "%s лимит: %.2f EUR\n"},
    {"Is this a recurring transaction? (0=No, 1=Yes): ", "Повтаряща се транзакция? (0=Не, 1=Да): "},
    {"Recurrence type:", "Тип повторение:"},
    {"0=Daily, 1=Weekly, 2=Monthly, 3=Yearly", "0=Дневно, 1=Седмично, 2=Месечно, 3=Годишно"},
    {"Recurring transaction added!", "Повтаряща се транзакция добавена!"},
    {"Recurring transactions processed!", "Повтарящите се транзакции са обработени!"}
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
int is_date_in_range(const char* date, DateRange range);
void input_date_range(DateRange* range);
int input_int_range(const char* prompt, int min, int max);
int is_valid_date(int year, int month, int day);
void load_budget_limits();
void save_budget_limits();
float input_positive_float(const char* prompt);
void load_recurring_transactions();
void save_recurring_transactions();
void process_recurring_transactions();
void add_next_date(char* current_date, RecurrenceType type, char* next_date);
int compare_dates(const char* date1, const char* date2);

int main(void) {
    initialize_settings();
    load_data();
    load_budget_limits();
    load_recurring_transactions();
    
    // Process recurring transactions on startup
    process_recurring_transactions();
    
    int choice;
    
    while (1) {
        clear_screen();
        print_menu();
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
                save_recurring_transactions();
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
        return;
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

void load_recurring_transactions() {
    FILE* file = fopen(RECURRING_FILE, "r");
    if (file == NULL) {
        return;
    }
    
    recurring_count = 0;
    while (fscanf(file, "%d %s %s %f %d %d %d %d %d",
                  &recurring_transactions[recurring_count].id,
                  recurring_transactions[recurring_count].start_date,
                  recurring_transactions[recurring_count].last_processed,
                  &recurring_transactions[recurring_count].amount,
                  (int*)&recurring_transactions[recurring_count].currency,
                  (int*)&recurring_transactions[recurring_count].type,
                  (int*)&recurring_transactions[recurring_count].category,
                  (int*)&recurring_transactions[recurring_count].recurrence,
                  &recurring_transactions[recurring_count].active) == 9) {
        recurring_count++;
        if (recurring_count >= MAX_RECURRING) break;
    }
    
    fclose(file);
}

void save_recurring_transactions() {
    FILE* file = fopen(RECURRING_FILE, "w");
    if (file == NULL) {
        return;
    }
    
    for (int i = 0; i < recurring_count; i++) {
        fprintf(file, "%d %s %s %.2f %d %d %d %d %d\n",
                recurring_transactions[i].id,
                recurring_transactions[i].start_date,
                recurring_transactions[i].last_processed,
                recurring_transactions[i].amount,
                recurring_transactions[i].currency,
                recurring_transactions[i].type,
                recurring_transactions[i].category,
                recurring_transactions[i].recurrence,
                recurring_transactions[i].active);
    }
    
    fclose(file);
}

int compare_dates(const char* date1, const char* date2) {
    int y1, m1, d1, y2, m2, d2;
    sscanf(date1, "%d-%d-%d", &y1, &m1, &d1);
    sscanf(date2, "%d-%d-%d", &y2, &m2, &d2);
    
    int val1 = y1 * 10000 + m1 * 100 + d1;
    int val2 = y2 * 10000 + m2 * 100 + d2;
    
    if (val1 < val2) return -1;
    if (val1 > val2) return 1;
    return 0;
}

void add_next_date(char* current_date, RecurrenceType type, char* next_date) {
    int y, m, d;
    sscanf(current_date, "%d-%d-%d", &y, &m, &d);
    
    struct tm date = {0};
    date.tm_year = y - 1900;
    date.tm_mon = m - 1;
    date.tm_mday = d;
    
    time_t t = mktime(&date);
    
    switch(type) {
        case DAILY:
            t += 24 * 60 * 60;
            break;
        case WEEKLY:
            t += 7 * 24 * 60 * 60;
            break;
        case MONTHLY:
            t += 30 * 24 * 60 * 60;
            break;
        case YEARLY:
            t += 365 * 24 * 60 * 60;
            break;
    }
    
    struct tm* new_date = localtime(&t);
    sprintf(next_date, "%04d-%02d-%02d", 
            new_date->tm_year + 1900, 
            new_date->tm_mon + 1, 
            new_date->tm_mday);
}

void process_recurring_transactions() {
    char today[11];
    get_current_date(today);
    
    int added = 0;
    
    for (int i = 0; i < recurring_count; i++) {
        if (!recurring_transactions[i].active) continue;
        
        char next_date[11];
        strcpy(next_date, recurring_transactions[i].last_processed);
        
        while (1) {
            add_next_date(next_date, recurring_transactions[i].recurrence, next_date);
            
            if (compare_dates(next_date, today) > 0) break;
            
            if (transaction_count >= MAX_TRANSACTIONS) break;
            
            Transaction new_trans;
            new_trans.id = transaction_count + 1;
            strcpy(new_trans.date, next_date);
            new_trans.amount = recurring_transactions[i].amount;
            new_trans.currency = recurring_transactions[i].currency;
            new_trans.type = recurring_transactions[i].type;
            new_trans.category = recurring_transactions[i].category;
            
            transactions[transaction_count++] = new_trans;
            strcpy(recurring_transactions[i].last_processed, next_date);
            added++;
        }
    }
    
    if (added > 0) {
        save_data();
        save_recurring_transactions();
        printf("\n%s\n", get_string(RECURRING_PROCESSED));
    }
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
    while (1) {
        if (scanf("%d", &type_input) != 1 || (type_input != 0 && type_input != 1)) {
            while (getchar() != '\n');
            printf("%s\n", get_string(INVALID_INPUT));
            printf("%s", get_string(TYPE_PROMPT));
            continue;
        }
        break;
    }
    new_trans.type = (TransactionType)type_input;
    
    printf("%s", get_string(AMOUNT_PROMPT));
    while (1) {
        if (scanf("%f", &new_trans.amount) != 1 || new_trans.amount <= 0) {
            while (getchar() != '\n');
            printf("%s\n", get_string(INVALID_INPUT));
            printf("%s", get_string(AMOUNT_PROMPT));
            continue;
        }
        break;
    }
    
    printf("%s", get_string(CURRENCY_PROMPT));
    int curr_input;
    while (1) {
        if (scanf("%d", &curr_input) != 1 || (curr_input != 0 && curr_input != 1)) {
            while (getchar() != '\n');
            printf("%s\n", get_string(INVALID_INPUT));
            printf("%s", get_string(CURRENCY_PROMPT));
            continue;
        }
        break;
    }
    new_trans.currency = (Currency)curr_input;
    
    printf("\n%s\n", get_string(CATEGORY_PROMPT));
    printf("%s\n", get_string(CATEGORY_OPTIONS));
    int cat_input;
    while (1) {
        if (scanf("%d", &cat_input) != 1 || cat_input < 0 || cat_input > 5) {
            while (getchar() != '\n');
            printf("%s\n", get_string(INVALID_INPUT));
            printf("\n%s\n", get_string(CATEGORY_PROMPT));
            printf("%s\n", get_string(CATEGORY_OPTIONS));
            continue;
        }
        break;
    }
    new_trans.category = (Category)cat_input;

    // Budget limit checks
    if (new_trans.type == EXPENSE) {
        float total_expenses = 0;
        for (int i = 0; i < transaction_count; i++) {
            if (transactions[i].type == EXPENSE) {
                total_expenses += convert_currency(transactions[i].amount, transactions[i].currency, EUR);
            }
        }
        float new_amount_eur = convert_currency(new_trans.amount, new_trans.currency, EUR);
        if (budget_limits.total_limit > 0 && (total_expenses + new_amount_eur) > budget_limits.total_limit) {
            printf("%s\n", get_string(TOTAL_LIMIT_EXCEEDED));
            printf("%s", get_string(PRESS_ENTER));
            wait_for_enter();
            return;
        }
        
        float cat_total = 0;
        for (int i = 0; i < transaction_count; i++) {
            if (transactions[i].type == EXPENSE && transactions[i].category == new_trans.category) {
                cat_total += convert_currency(transactions[i].amount, transactions[i].currency, EUR);
            }
        }
        if (budget_limits.category_limits[new_trans.category] > 0 &&
            (cat_total + new_amount_eur) > budget_limits.category_limits[new_trans.category]) {
            printf("%s\n", get_string(CATEGORY_LIMIT_EXCEEDED));
            printf("%s", get_string(PRESS_ENTER));
            wait_for_enter();
            return;
        }
    }
    
    while(getchar() != '\n');
    
    transactions[transaction_count++] = new_trans;
    
    printf("\n%s\n", get_string(TRANSACTION_ADDED));

        // Ask if recurring
    printf("\n%s", get_string(IS_RECURRING_PROMPT));
    int is_recurring;
    while (1) {
        if (scanf("%d", &is_recurring) != 1 || (is_recurring != 0 && is_recurring != 1)) {
            while (getchar() != '\n');
            printf("%s\n", get_string(INVALID_INPUT));
            printf("%s", get_string(IS_RECURRING_PROMPT));
            continue;
        }
        break;
    }
    
    if (is_recurring) {
        if (recurring_count >= MAX_RECURRING) {
            printf("%s\n", get_string(INVALID_INPUT));
            while(getchar() != '\n');
            return;
        }
        
        printf("\n%s\n", get_string(RECURRENCE_TYPE_PROMPT));
        printf("%s\n", get_string(RECURRENCE_OPTIONS));
        int rec_input;
        while (1) {
            if (scanf("%d", &rec_input) != 1 || rec_input < 0 || rec_input > 3) {
                while (getchar() != '\n');
                printf("%s\n", get_string(INVALID_INPUT));
                printf("\n%s\n", get_string(RECURRENCE_TYPE_PROMPT));
                printf("%s\n", get_string(RECURRENCE_OPTIONS));
                continue;
            }
            break;
        }
        
        RecurringTransaction rec_trans;
        rec_trans.id = recurring_count + 1;
        strcpy(rec_trans.start_date, new_trans.date);
        strcpy(rec_trans.last_processed, new_trans.date);
        rec_trans.amount = new_trans.amount;
        rec_trans.currency = new_trans.currency;
        rec_trans.type = new_trans.type;
        rec_trans.category = new_trans.category;
        rec_trans.recurrence = (RecurrenceType)rec_input;
        rec_trans.active = 1;
        
        recurring_transactions[recurring_count++] = rec_trans;
        
        printf("\n%s\n", get_string(RECURRING_ADDED));
    }
    printf("%s\n", get_string(PRESS_ENTER));
    wait_for_enter();
}

int is_date_in_range(const char* date, DateRange r) {
    int y, m, d;
    sscanf(date, "%d-%d-%d", &y, &m, &d);

    int start = r.from_year * 10000 + r.from_month * 100 + r.from_day;
    int end   = r.to_year   * 10000 + r.to_month   * 100 + r.to_day;
    int curr  = y * 10000 + m * 100 + d;

    return curr >= start && curr <= end;
}

void view_transactions() {
    printf("\n══════════════════════════════════════════════════\n");
    printf("%s\n", get_string(ALL_TRANSACTIONS));
    printf("══════════════════════════════════════════════════\n");

    if (transaction_count == 0) {
        printf("%s\n", get_string(NO_TRANSACTIONS));
        printf("%s\n", get_string(PRESS_ENTER));
        return;
    }

    printf("\n%s\n", get_string(FILTER_CHOICE));
    printf("%s\n", get_string(SHOW_ALL_OPTION));
    printf("%s\n", get_string(FILTER_OPTION));

    int mode = input_int_range(get_string(ENTER_CHOICE), 0, 1);

    DateRange range;
    if (mode == FILTER_BY_DATE) {
        input_date_range(&range);
    }

    int found = 0;
    for (int i = 0; i < transaction_count; i++) {
        if (mode == FILTER_BY_DATE && !is_date_in_range(transactions[i].date, range))
            continue;

        Transaction t = transactions[i];
        if(found == 0){
           printf("[%s]   %s    |    %s   | %s | %s \n", get_string(ID_TEXT), get_string(DATE_TEXT),
           get_string(AMOUNT_TEXT), get_string(TYPE_TEXT), get_string(CATEGORY_TEXT));
           found = 1;
        }

        printf("[%d] %s | %.2f %s | %s | %s\n",
               t.id,
               t.date,
               t.amount,
               currency_symbols[t.currency],
               t.type == INCOME ? "+" : "-",
               category_names[t.category][settings.language]);
    }

    if (!found) {
        printf("%s\n", get_string(NO_TRANSACTIONS));
    }

    printf("%s\n", get_string(PRESS_ENTER));
}

float convert_currency(float amount, Currency from, Currency to) {
    if (from == to) return amount;
    
    if (from == EUR && to == BGN) {
        return amount * EXCHANGE_RATE;
    } else {
        return amount / EXCHANGE_RATE;
    }
}

void view_summary() {
    printf("\n══════════════════════════════════════════════════\n");
    const char* summary_title = (settings.language == ENGLISH) ? "Summary" : "Обобщение";
    printf("%s\n", summary_title);
    printf("══════════════════════════════════════════════════\n");

    printf("\n%s\n", get_string(FILTER_CHOICE));
    printf("%s\n", get_string(SHOW_ALL_OPTION));
    printf("%s\n", get_string(FILTER_OPTION));

    int mode = input_int_range(get_string(ENTER_CHOICE), 0, 1);

    DateRange range;
    if (mode == FILTER_BY_DATE) {
        input_date_range(&range);
    }

    float total_income_eur = 0, total_expense_eur = 0;

    for (int i = 0; i < transaction_count; i++) {
        if (mode == FILTER_BY_DATE && !is_date_in_range(transactions[i].date, range))
            continue;

        float amount_eur = convert_currency(transactions[i].amount, transactions[i].currency, EUR);

        if (transactions[i].type == INCOME) {
            total_income_eur += amount_eur;
        } else {
            total_expense_eur += amount_eur;
        }
    }

    float balance_eur = total_income_eur - total_expense_eur;

    printf("\n%s %.2f EUR (%.2f BGN)\n", get_string(TOTAL_INCOME),
           total_income_eur, convert_currency(total_income_eur, EUR, BGN));
    printf("%s %.2f EUR (%.2f BGN)\n", get_string(TOTAL_EXPENSES),
           total_expense_eur, convert_currency(total_expense_eur, EUR, BGN));
    printf("%s %.2f EUR (%.2f BGN)\n", get_string(BALANCE),
           balance_eur, convert_currency(balance_eur, EUR, BGN));

    if (total_expense_eur > 0) {
        const char* expenses_title = (settings.language == ENGLISH) ? "\n--- Expenses by Category ---\n" : "\n--- Разходи по категория ---\n";
        printf("%s\n", expenses_title);

        for (int cat = 0; cat <= OTHER; cat++) {
            float cat_total = 0;
            for (int i = 0; i < transaction_count; i++) {
                if (transactions[i].type == EXPENSE &&
                    transactions[i].category == cat &&
                    (mode == SHOW_ALL || is_date_in_range(transactions[i].date, range))) {
                    cat_total += convert_currency(transactions[i].amount, transactions[i].currency, EUR);
                }
            }

            if (cat_total > 0) {
                float percentage = (cat_total / total_expense_eur) * 100;
                printf("%s: %.2f EUR (%.1f%%)\n",
                       category_names[cat][settings.language],
                       cat_total, percentage);
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
        printf("%s\n", get_string(SET_BUDGET_LIMITS));
        printf("%s\n\n", get_string(BACK_TO_MENU));
        
        printf("%s%s\n", get_string(CURRENT_LANGUAGE), get_string(LANG_ENGLISH + settings.language));
        printf("%s\n\n", get_string(EXCHANGE_RATE_TEXT));
        printf("%s\n", get_string(CURRENT_BUDGET_LIMITS));
        printf(get_string(CURRENT_TOTAL_LIMIT), budget_limits.total_limit);
        for (int i = 0; i <= OTHER; i++) {
            printf(get_string(CURRENT_CATEGORY_LIMIT), category_names[i][settings.language], budget_limits.category_limits[i]); 
        }
        printf("\n");
        
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
            case 2: {
                printf("\n%s\n", get_string(SET_LIMITS_PROMPT));

                budget_limits.total_limit = input_positive_float(get_string(TOTAL_LIMIT_PROMPT));

                for (int i = 0; i <= OTHER; i++) {
                    char prompt[MAX_STRING];
                    sprintf(prompt, "%s: ", category_names[i][settings.language]);
                    budget_limits.category_limits[i] = input_positive_float(prompt);
                }

                save_budget_limits(); 
                printf("%s\n", get_string(PRESS_ENTER)); 
                wait_for_enter(); 
                break;
            }
            case 3:
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

float input_positive_float(const char* prompt) {
    float x;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &x) != 1 || x < 0) {
            while (getchar() != '\n');
            printf("%s\n", get_string(INVALID_INPUT));
            continue;
        }
        while (getchar() != '\n');
        return x;
    }
}

void wait_for_enter() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int input_int_range(const char* prompt, int min, int max) {
    int x;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &x) != 1 || x < min || x > max) {
            while (getchar() != '\n');
            printf("%s\n", get_string(INVALID_INPUT));
            continue;
        }
        while (getchar() != '\n');
        return x;
    }
}

void input_date_range(DateRange* r) {
    while (1) {
        r->from_year  = input_int_range(get_string(ENTER_FROM_YEAR), 1900, 2100);
        r->from_month = input_int_range(get_string(ENTER_FROM_MONTH), 1, 12);
        r->from_day   = input_int_range(get_string(ENTER_FROM_DAY), 1, 31);

        if (!is_valid_date(r->from_year, r->from_month, r->from_day)) {
            printf("%s\n", get_string(INVALID_DATE));
            continue;
        }

        r->to_year  = input_int_range(get_string(ENTER_TO_YEAR), 1900, 2100);
        r->to_month = input_int_range(get_string(ENTER_TO_MONTH), 1, 12);
        r->to_day   = input_int_range(get_string(ENTER_TO_DAY), 1, 31);
        
        if (!is_valid_date(r->to_year, r->to_month, r->to_day)) {
            printf("%s\n", get_string(INVALID_DATE));
            continue;
        }

        int start = r->from_year * 10000 + r->from_month * 100 + r->from_day;
        int end   = r->to_year   * 10000 + r->to_month   * 100 + r->to_day;

        if (end < start) {
            printf("%s\n", get_string(END_DATE_BEFORE_START));
            continue;
        }
        break;
    }
}

int is_valid_date(int year, int month, int day) {
    if (month < 1 || month > 12 || day < 1) return 0;

    int days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if (month == 2) {
        int leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if (leap) days_in_month[1] = 29;
    }

    if (day > days_in_month[month - 1]) return 0;

    return 1;
}

void load_budget_limits() {
    FILE* file = fopen("budget_limits.txt", "r");
    if (!file) return;

    fscanf(file, "%f", &budget_limits.total_limit);
    for (int i = 0; i <= OTHER; i++) {
        fscanf(file, "%f", &budget_limits.category_limits[i]);
    }

    fclose(file);
}

void save_budget_limits() {
    FILE* file = fopen("budget_limits.txt", "w");
    if (!file) return;

    fprintf(file, "%.2f\n", budget_limits.total_limit);
    for (int i = 0; i <= OTHER; i++) {
        fprintf(file, "%.2f\n", budget_limits.category_limits[i]);
    }

    fclose(file);
}