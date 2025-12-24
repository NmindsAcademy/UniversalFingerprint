#!/usr/bin/env python3
"""
Database Converter for UniversalFingerprint Library

This tool converts fingerprint database formats between different sensors.
Currently supports conversion between AS608 and R307 formats.

Usage:
    python DatabaseConverter.py --input input.bin --output output.bin --from as608 --to r307
"""

import argparse
import struct
import sys
from typing import Dict, List, Tuple

class DatabaseConverter:
    """Convert fingerprint databases between different formats."""
    
    # Sensor formats and their characteristics
    SENSOR_FORMATS = {
        'as608': {
            'name': 'AS608',
            'capacity': 162,
            'template_size': 512,  # Approximate size in bytes
            'header_size': 6,
        },
        'r307': {
            'name': 'R307',
            'capacity': 1000,
            'template_size': 512,  # May vary
            'header_size': 6,
        },
        'gt511c3': {
            'name': 'GT-511C3',
            'capacity': 200,
            'template_size': 1024,  # Larger templates
            'header_size': 8,
        }
    }
    
    def __init__(self):
        self.templates = []
        self.metadata = {}
        
    def load_database(self, filename: str, format_name: str) -> bool:
        """Load database from file."""
        try:
            with open(filename, 'rb') as f:
                data = f.read()
            
            format_info = self.SENSOR_FORMATS.get(format_name.lower())
            if not format_info:
                print(f"Error: Unknown format '{format_name}'")
                return False
            
            print(f"Loading {format_info['name']} database from {filename}")
            print(f"File size: {len(data)} bytes")
            
            # Parse based on format
            if format_name.lower() == 'as608':
                return self._parse_as608(data)
            elif format_name.lower() == 'r307':
                return self._parse_r307(data)
            elif format_name.lower() == 'gt511c3':
                return self._parse_gt511c3(data)
            else:
                print(f"Parser not implemented for {format_name}")
                return False
                
        except FileNotFoundError:
            print(f"Error: File '{filename}' not found")
            return False
        except Exception as e:
            print(f"Error loading database: {e}")
            return False
    
    def _parse_as608(self, data: bytes) -> bool:
        """Parse AS608 format database."""
        # AS608 format: Simple sequential storage
        template_size = 512  # Approximate
        
        self.templates = []
        for i in range(0, len(data), template_size):
            chunk = data[i:i+template_size]
            if len(chunk) == template_size and any(chunk):  # Non-zero template
                template_id = (i // template_size) + 1
                self.templates.append({
                    'id': template_id,
                    'data': chunk,
                    'size': len(chunk)
                })
        
        print(f"Found {len(self.templates)} templates")
        return True
    
    def _parse_r307(self, data: bytes) -> bool:
        """Parse R307 format database."""
        # R307 format: May have index table
        # Simplified: same as AS608 for now
        return self._parse_as608(data)
    
    def _parse_gt511c3(self, data: bytes) -> bool:
        """Parse GT-511C3 format database."""
        # GT-511C3 has larger templates
        template_size = 1024
        
        self.templates = []
        for i in range(0, len(data), template_size):
            chunk = data[i:i+template_size]
            if len(chunk) == template_size and any(chunk):
                template_id = (i // template_size) + 1
                self.templates.append({
                    'id': template_id,
                    'data': chunk,
                    'size': len(chunk)
                })
        
        print(f"Found {len(self.templates)} templates")
        return True
    
    def save_database(self, filename: str, format_name: str) -> bool:
        """Save database to file in specified format."""
        try:
            format_info = self.SENSOR_FORMATS.get(format_name.lower())
            if not format_info:
                print(f"Error: Unknown format '{format_name}'")
                return False
            
            print(f"Saving {len(self.templates)} templates as {format_info['name']} database")
            
            # Convert templates to target format
            output_data = bytearray()
            
            if format_name.lower() == 'as608':
                output_data = self._convert_to_as608()
            elif format_name.lower() == 'r307':
                output_data = self._convert_to_r307()
            elif format_name.lower() == 'gt511c3':
                output_data = self._convert_to_gt511c3()
            else:
                print(f"Converter not implemented for {format_name}")
                return False
            
            # Write to file
            with open(filename, 'wb') as f:
                f.write(output_data)
            
            print(f"Database saved to {filename}")
            print(f"Output size: {len(output_data)} bytes")
            return True
            
        except Exception as e:
            print(f"Error saving database: {e}")
            return False
    
    def _convert_to_as608(self) -> bytearray:
        """Convert templates to AS608 format."""
        output = bytearray()
        for template in self.templates:
            # Ensure template is correct size
            data = template['data']
            if len(data) > 512:
                data = data[:512]  # Truncate
            elif len(data) < 512:
                data = data + bytes(512 - len(data))  # Pad
            
            output.extend(data)
        
        # Pad to full capacity
        full_size = 162 * 512  # AS608 capacity
        if len(output) < full_size:
            output.extend(bytes(full_size - len(output)))
        
        return output
    
    def _convert_to_r307(self) -> bytearray:
        """Convert templates to R307 format."""
        # Similar to AS608 but with more capacity
        output = bytearray()
        for template in self.templates:
            data = template['data']
            if len(data) > 512:
                data = data[:512]
            elif len(data) < 512:
                data = data + bytes(512 - len(data))
            
            output.extend(data)
        
        # R307 has 1000 slots
        full_size = 1000 * 512
        if len(output) < full_size:
            output.extend(bytes(full_size - len(output)))
        
        return output
    
    def _convert_to_gt511c3(self) -> bytearray:
        """Convert templates to GT-511C3 format."""
        output = bytearray()
        for template in self.templates:
            data = template['data']
            if len(data) > 1024:
                data = data[:1024]
            elif len(data) < 1024:
                data = data + bytes(1024 - len(data))
            
            output.extend(data)
        
        # GT-511C3 has 200 slots
        full_size = 200 * 1024
        if len(output) < full_size:
            output.extend(bytes(full_size - len(output)))
        
        return output
    
    def print_statistics(self):
        """Print database statistics."""
        print("\n=== Database Statistics ===")
        print(f"Total templates: {len(self.templates)}")
        
        if self.templates:
            sizes = [t['size'] for t in self.templates]
            print(f"Average template size: {sum(sizes) // len(sizes)} bytes")
            print(f"Min template size: {min(sizes)} bytes")
            print(f"Max template size: {max(sizes)} bytes")
            
            # Template IDs
            ids = [t['id'] for t in self.templates]
            print(f"Template IDs: {sorted(ids)}")
        
        total_size = sum(t['size'] for t in self.templates)
        print(f"Total data size: {total_size} bytes")
    
    def validate_templates(self) -> List[Tuple[int, str]]:
        """Validate template data integrity."""
        issues = []
        
        for template in self.templates:
            # Check for all zeros (empty template)
            if all(b == 0 for b in template['data']):
                issues.append((template['id'], "Empty template (all zeros)"))
            
            # Check for all 0xFF (erased flash)
            if all(b == 0xFF for b in template['data']):
                issues.append((template['id'], "Erased template (all 0xFF)"))
            
            # Check size
            if template['size'] < 100:
                issues.append((template['id'], f"Template too small ({template['size']} bytes)"))
        
        return issues

def main():
    parser = argparse.ArgumentParser(
        description='Convert fingerprint databases between sensor formats'
    )
    parser.add_argument('--input', required=True, help='Input database file')
    parser.add_argument('--output', required=True, help='Output database file')
    parser.add_argument('--from', dest='input_format', required=True, 
                       choices=['as608', 'r307', 'gt511c3'], help='Input format')
    parser.add_argument('--to', dest='output_format', required=True,
                       choices=['as608', 'r307', 'gt511c3'], help='Output format')
    parser.add_argument('--stats', action='store_true', help='Print statistics')
    parser.add_argument('--validate', action='store_true', help='Validate templates')
    
    args = parser.parse_args()
    
    converter = DatabaseConverter()
    
    # Load input database
    if not converter.load_database(args.input, args.input_format):
        sys.exit(1)
    
    # Print statistics if requested
    if args.stats:
        converter.print_statistics()
    
    # Validate if requested
    if args.validate:
        issues = converter.validate_templates()
        if issues:
            print("\n=== Validation Issues ===")
            for template_id, issue in issues:
                print(f"Template {template_id}: {issue}")
        else:
            print("\nNo validation issues found")
    
    # Convert and save
    if not converter.save_database(args.output, args.output_format):
        sys.exit(1)
    
    print("\nConversion completed successfully!")

if __name__ == '__main__':
    main()