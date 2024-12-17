import tkinter as tk
from tkinter import ttk, messagebox
import requests
from tkhtmlview import HTMLLabel
from PIL import Image, ImageTk
from io import BytesIO

# Функция для получения и отображения HTML
def get_weather():
    city = city_entry.get()
    if not city:
        messagebox.showwarning("Ошибка", "Пожалуйста, введите название города.")
        return

    try:
        # Отправка запроса на сервер
        url = f"http://localhost:3000?city={city}"
        response = requests.get(url)

        if response.status_code == 200:
            # Получаем HTML-контент
            html_content = response.text

            # Изменяем теги <img> для корректного отображения
            html_content = html_content.replace('<img', '<img width="80" height="80"')

            # Отображаем HTML в HTMLLabel
            html_label.set_html(html_content)
        else:
            messagebox.showerror("Ошибка", "Не удалось получить данные о погоде.")
    except requests.exceptions.RequestException as e:
        messagebox.showerror("Ошибка", f"Ошибка сети: {e}")

# Создание главного окна
root = tk.Tk()
root.geometry("400x500")

# Стили и виджеты
city_label = ttk.Label(root, text="Введите город:", font=("Arial", 12))
city_label.pack(pady=10)

city_entry = ttk.Entry(root, width=30, font=("Arial", 12))
city_entry.pack(pady=5)

get_weather_button = ttk.Button(root, text="Получить погоду", command=get_weather)
get_weather_button.pack(pady=10)

# HTMLLabel для отображения содержимого HTML
html_label = HTMLLabel(root, html="<p>Здесь будет прогноз погоды</p>", width=400, height=300)
html_label.pack(pady=20, fill="both", expand=True)

# Подпись
footer_label = ttk.Label(root, text="Данные предоставлены OpenWeather", font=("Arial", 8), foreground="gray")
footer_label.pack(side="bottom", pady=5)

# Запуск приложения
root.mainloop()
