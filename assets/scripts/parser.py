import json
import re
from pathlib import Path


unwanted_machines = {
    "BP_WorkBenchComponent_C",
    "FGBuildableAutomatedWorkBench",
    "Build_AutomatedWorkBench_C",
    "BP_WorkshopComponent_C",
    "BP_BuildGun_C",
    "FGBuildGun"
}


def parse_item_amounts(raw_string):
    if not raw_string:
        return []

    items = []
    matches = re.findall(r'ItemClass="(.*?)",Amount=(\d+)', raw_string)

    for item_path, amount_str in matches:
        clean_name = re.split(r'[./]', item_path)[-1].strip("'\"")
        items.append({
            "Item": clean_name,
            "Amount": int(amount_str)
        })

    return items


def parse_class_list(raw_string):
    if not raw_string:
        return []
    matches = re.findall(r"'([^']+)'", raw_string)
    return [path.split('.')[-1] for path in matches]


def parse_enum_list(raw_string):
    if not raw_string:
        return []
    return raw_string.strip("()").split(",")


def process_item(item, is_resource=False):
    return {
        "Class": item.get("ClassName"),
        "DisplayName": item.get("mDisplayName"),
        "SinkPoints": item.get("mResourceSinkPoints"),
        "Icon": item.get("mSmallIcon"),
        "Form": item.get("mForm"),
        "isResource": is_resource
    }



def process_recipe(recipe):

    raw_produced_in = recipe.get("mProducedIn", "")
    produced_in_list = re.findall(r'"([^"]+)"', raw_produced_in)
    cleaned_list = [
        item.split('.')[-1]
        for item in produced_in_list
        if item.split('.')[-1] not in unwanted_machines
    ]
    if cleaned_list:
        className = recipe.get("ClassName")
        if (len(cleaned_list) > 1):
            raise ValueError

        return {
            "Class": className,
            "DisplayName": recipe.get("mDisplayName"),
            "Alternate": "Alternate" in className,
            "Ingredients": parse_item_amounts(recipe.get("mIngredients")),
            "Product": parse_item_amounts(recipe.get("mProduct")),
            "ManufactoringDuration": recipe.get("mManufactoringDuration"),
            "ProducedIn": cleaned_list[0]
        }

    return None


def process_machine(machine, machineType: str):
    return {
        "Class": machine.get("ClassName"),
        "DisplayName": machine.get("mDisplayName"),
        "PowerConsumption": machine.get("mPowerConsumption"),
        "SomersloopSlotSize": machine.get("mProductionShardSlotSize"),
        "_MachineType:": machineType
    }


def process_extractors(extractor, machineType: str):
    displayName = extractor.get("mDisplayName")
    extTypeName = extractor.get("mExtractorTypeName")
    if (extTypeName == "None"):
        extTypeName = displayName.replace(" ", "_")
    return {
        "Class": extractor.get("ClassName"),
        "DisplayName": displayName,
        "ExtractorTypeName": extTypeName,
        "ExtractCycleTime": extractor.get("mExtractCycleTime"),
        "ItemsPerCycle": extractor.get("mItemsPerCycle"),
        "OnlyAllowCertainResources": extractor.get("mOnlyAllowCertainResources") == "True",
        "AllowedResourceForms": parse_enum_list(extractor.get("mAllowedResourceForms")),
        "AllowedResources": parse_class_list(extractor.get("mAllowedResources")),
        "PowerConsumption": extractor.get("mPowerConsumption"),
        "PowerConsumptionExponent": extractor.get("mPowerConsumptionExponent"),
        "ProductionBoostPowerConsumptionExponent": extractor.get("mProductionBoostPowerConsumptionExponent"),
        "_MachineType": machineType
    }


def find_classes(data, class_name):
    search_terms = [class_name] if isinstance(class_name, str) else class_name
    results = []

    for entry in data:
        native_class_str = entry.get("NativeClass", "")
        if any(term in native_class_str for term in search_terms):
            results.extend(entry.get("Classes", []))

    return results


def main():
    scripts_dir = Path(__file__).parent
    json_dir = scripts_dir.parent / "jsons"
    docs = json_dir / "Docs.json"
    with open(docs, 'r', encoding="utf-16") as f:
        data = json.load(f)

    # --- ITEMS ---
    raw_items = find_classes(data, "FGItemDescriptor")
    items = [process_item(item) for item in raw_items]
    raw_resources = find_classes(data, "FGResourceDescriptor")
    resources = [process_item(item, is_resource=True) for item in raw_resources]

    all_items = {
        "Components": items,
        "Resources": resources
    }

    with open(json_dir / "items.json", 'w', encoding="utf-8") as f:
        json.dump(all_items, f, indent=4)
    print(f"Extracted {len(items)} items to items.json")

    # --- RECIPES ---
    raw_recipes = find_classes(data, "FGRecipe")
    recipes = [r for recipe in raw_recipes if (r := process_recipe(recipe)) is not None]

    with open(json_dir / "recipes.json", 'w', encoding="utf-8") as f:
        json.dump(recipes, f, indent=4)
    print(f"Extracted {len(recipes)} recipes to recipes.json")

    # --- MACHINES ---
    raw_machines = find_classes(data,
                                ["FGBuildableManufacturer", "FGBuildableManufacturerVariablePower"])
    raw_extractors = find_classes(data,
                                  ["FGBuildableResourceExtractor", "FGBuildableWaterPump"])
    machines = [process_machine(machine, "Production") for machine in raw_machines]
    extractors = [process_extractors(extractor, "Extraction") for extractor in raw_extractors]
    all_machines = {
        "ProductionMachines": machines,
        "ExtractionMachines": extractors
    }

    with open(json_dir / "machines.json", 'w', encoding="utf-8") as f:
        json.dump(all_machines, f, indent=4)
    print(f"Extracted {len(machines) + len(extractors)} machines to machines.json")


if __name__ == "__main__":
    main()
