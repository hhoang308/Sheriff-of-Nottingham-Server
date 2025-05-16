import discord
from discord.ext import commands
import os
from dotenv import load_dotenv
import glob
import subprocess


load_dotenv()
TOKEN = os.getenv('BOT_TOKEN')

intents = discord.Intents.default()
intents.message_content = True


bot = commands.Bot(command_prefix='!', intents=intents)

server_is_running = None

def get_latest_log_file(log_dir='../build/log'):
    list_of_files = glob.glob(os.path.join(log_dir, 'log_*.txt'))
    if not list_of_files:
        return None
    latest_file = max(list_of_files, key=os.path.getmtime)
    return latest_file

@bot.event
async def on_ready():
    print(f'{bot.user.name} has connected to Discord!')

@bot.command(name='start', help='Run Sheriff of Nottingham Server')
async def start(ctx):
    global server_is_running
    if server_is_running is not None and server_is_running.poll() is None:
        await ctx.send("⚠️ Server is already running.")
        return

    try:
        server_is_running = subprocess.Popen(["./build/SheriffOfNottinghamServer"], cwd="..")
        await ctx.send("✅ Server script started.")
    except Exception as e:
        await ctx.send(f"❌ Failed to start script: {str(e)}")

@bot.command(name='status', help='Get status of Sheriff of Nottingham Server')
async def status(ctx):
    global server_is_running
    if server_is_running is not None and server_is_running.poll() is None:
        await ctx.send("🟢 Server is currently **running**.")
    else:
        await ctx.send("🔴 Server is **not running**.")

@bot.command(name='stop', help='Stop Sheriff of Nottingham Server')
async def stop(ctx):
    global server_is_running
    if server_is_running is None or server_is_running.poll() is not None:
        await ctx.send("⚠️ Server is not running.")
        return

    server_is_running.terminate()
    server_is_running.wait()
    await ctx.send("🛑 Server has been stopped.")

@bot.command(name='log', help='Stop Sheriff of Nottingham Server')
async def log(ctx):
    latest_log = get_latest_log_file()
    if latest_log:
        await ctx.send("📄 Here is the latest log file:", file=discord.File(latest_log))
    else:
        await ctx.send("❗No log file found.")

bot.run(TOKEN)